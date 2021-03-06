#include "sqrt.h"
#include <abycore/circuit/booleancircuits.h>
#include <abycore/sharing/sharing.h>

int32_t test_sqrt_circuit(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t nvals, uint32_t nthreads, e_mt_gen_alg mt_alg,
		e_sharing sharing) {

	uint8_t bitlen = 64;
	/**
		Step 1: Create the ABYParty object which defines the basis of all the
		 	 	operations which are happening.	Operations performed are on the
		 	 	basis of the role played by this object.
	*/
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads, mt_alg);

	/**
		Step 2: Get to know all the sharing types available in the program.
	*/

	std::vector<Sharing*>& sharings = party->GetSharings();

	/**
		Step 3: Create the circuit object on the basis of the sharing type
				being inputed.
	*/
	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();


	/**
		Step 4: Creating the share objects - s_A and s_B which
				is used as input to the computation function. Also s_out
				which stores the output.
	*/

	share *s_A, *s_B, *s_out;

	/**
		Step 5: Initialize Alice's and Bob's money with random values.
				Both parties use the same seed, to be able to verify the
				result. In a real example each party would only supply
				one input value.
	*/

	uint32_t A, B, output;
	// Evaluator inputs
	double a = 1.4;
	uint64_t *a_pointer = (uint64_t*) &a;
	uint64_t input_a = *a_pointer;

	double b = 4.66;
	uint64_t *b_pointer = (uint64_t*) &b;
	uint64_t input_b = *b_pointer;

	if(role == SERVER) {

		s_A = circ->PutINGate(input_a, bitlen, SERVER);
		s_B = circ->PutDummyINGate(bitlen);
	} else { //role == Evaluator
		s_A = circ->PutDummyINGate(bitlen);
		s_B = circ->PutINGate(input_b, bitlen, CLIENT);
	}

	/**
		Step 7: Call the build method for building the circuit for the
				problem by passing the shared objects and circuit object.
				Don't forget to type cast the circuit object to type of share
	*/

	s_out = BuildSqrtCircuit(s_A, s_B, (BooleanCircuit*) circ, bitlen);

	/**
		Step 8: Modify the output receiver based on the role played by
				the server and the client. This step writes the output to the
				shared output object based on the role.
	*/
	s_out = circ->PutOUTGate(s_out, ALL);

	/**
		Step 9: Executing the circuit using the ABYParty object evaluate the
				problem.
	*/
	party->ExecCircuit();

	/**
		Step 10:Type casting the value to 32 bit unsigned integer for output.
	*/
	
	// retrieve plain text output
	uint32_t out_bitlen, out_nvals;
	uint64_t *out_vals;
	s_out->get_clear_value_vec(&out_vals, &out_bitlen, &out_nvals);
	if (role == CLIENT) {
		// print every output
		for (uint32_t i = 0; i < nvals; i++) {
			// dereference output value as double without casting the content
			double val = *((double*) &out_vals[i]);
			std::cout << val << std::endl;
		}
	}
	return 0;
}

share* BuildSqrtCircuit(share *s_A, share *s_B, BooleanCircuit *bc, uint8_t bitlen) {

	// share* s_mult = bc->PutFPGate(s_A, SQR, no_status);

	share* out = bc->PutFPGate(s_A, s_B, MUL, bitlen, no_status);

	return out;
}
