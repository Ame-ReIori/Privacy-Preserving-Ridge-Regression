#include "circuit.h"
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
#include <abycore/circuit/booleancircuits.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <abycore/circuit/circuit.h>
#include <abycore/circuit/share.h>
#include <abycore/sharing/sharing.h>
#include <abycore/aby/abyparty.h>
#include <math.h>
#include <cassert>
#include <iomanip>
#include <iostream>
#include "utils.h"

void test_circuit(e_role role, const std::string& address, uint16_t port, seclvl seclvl, uint32_t nvals, uint32_t nthreads,
	e_mt_gen_alg mt_alg, e_sharing sharing, std::vector<double> data) {

	// we operate on doubles, so set bitlen to 64 bits
	uint32_t bitlen = 64;

	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);

	std::vector<Sharing*>& sharings = party->GetSharings();

	BooleanCircuit* bc = (BooleanCircuit*) sharings[S_BOOL]->GetCircuitBuildRoutine();
	ArithmeticCircuit* ac = (ArithmeticCircuit*) sharings[S_ARITH]->GetCircuitBuildRoutine();
	Circuit* yc = (Circuit*) sharings[S_YAO]->GetCircuitBuildRoutine();


	// DATA FORMATTING
	// -----------------------------------

	uint64_t input_csp[nvals];
	uint64_t input_eval[nvals];

	for(int i = 0; i < nvals; i++){
		double value = data[i];
		uint64_t *valuetr = (uint64_t*) &value;
		input_csp[i] = *valuetr;
		input_eval[i] = *valuetr;
	}

	// CIRCUIT INPUTS
	// -----------------------------------

	// SIMD input gates
	share* csp_in = bc->PutSIMDINGate(nvals, input_csp, bitlen, SERVER); // A + mu_a
	share* eval_in = bc->PutSIMDINGate(nvals, input_eval, bitlen, CLIENT); // mu_a

	// CIRCUIT OPERATIONS
	// -----------------------------------

	// FP substraction gate to remove mask mu_A from A + mu_a
	share* A = MatrixSubstraction(csp_in, eval_in, bc, nvals);

	// Putting a vector of zeros to initiate Lower decomposition of cholesky
	uint64_t constant = 5;
	uint64_t zeros[nvals] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint64_t zero = 0;

	share* L = ac->PutSIMDINGate(nvals, zeros, bitlen, SERVER); // zeros
	share* zero_share = bc->PutINGate(zero, bitlen, SERVER);

	A = ac->PutB2AGate(A);
	A = ac->PutSplitterGate(A);
	L = ac->PutSplitterGate(L);

	int n = sqrt(nvals); // number of lines (OK)
	for(int i=0; i<n; i++){
		share* mul = zero_share;
		for(int k=0; k<n; k++){
			uint32_t index = i*n+k;
			share* currentL = extract_index(L, index, bitlen, ac); //L[i*n+k]
			currentL = bc->PutY2BGate(yc->PutA2YGate(currentL)); //Converting to bc
			currentL = bc->PutFPGate(currentL, currentL, MUL, no_status); //currentL**2
			mul = bc->PutFPGate(mul, currentL, ADD, no_status); //mul += currentL**2
		}
		mul = ac->PutB2AGate(mul);
		A->set_wire_id(i, mul->get_wire_id(0));
	}

	A = ac->PutCombinerGate(A);
	// A = bc->PutY2BGate(yc->PutA2YGate(A));

	// CIRCUIT OUTPUTS
	// -----------------------------------
	share* res_out = ac->PutOUTGate(A, ALL);

	// run SMPC
	party->ExecCircuit();

	// // retrieve plain text output
	uint32_t out_bitlen, out_nvals;
	uint64_t *out_vals;
	res_out->get_clear_value_vec(&out_vals, &out_bitlen, &out_nvals);
	if (role == CLIENT) {
		// print every output
		for (uint32_t i = 0; i < nvals; i++) {
			// dereference output value as double without casting the content
			double val = *((double*) &out_vals[i]);
			std::cout << val << std::endl;
		}
	}


}

share* MatrixSubstraction(share *s_A, share *s_B, BooleanCircuit *bc, uint32_t nvals){
	share* out = bc->PutFPGate(s_A, s_B, SUB, nvals, no_status);
	return out;
}

share* extract_index(share *s_x , uint32_t i, uint32_t bitlen, ArithmeticCircuit *ac) 
{
	uint64_t zero = 0;
	share* out = ac->PutCONSGate(zero,bitlen);

	out->set_wire_id(0, s_x->get_wire_id(i));

	return out;
}

share* put_index(share *s_x , share *element, uint32_t i, uint32_t bitlen, ArithmeticCircuit *ac) 
{
	s_x->set_wire_id(i, element->get_wire_id(0));

	return s_x;
}