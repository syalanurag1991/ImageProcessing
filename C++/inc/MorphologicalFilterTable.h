///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// Author: Ganesh Ramdas Pillai, Anurag Syal ///////////////////////////////////////////////////////////////////////////////////////////////
///////// USC-ID: 9954-6583-64 (Anurag) ///////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// www.360otg.com //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>

#ifndef __MORPHOLOGICALFILTERTABLE_H__
#define __MORPHOLOGICALFILTERTABLE_H__

using namespace std;

/////////////////////////////////////////////	MORPHOLIGICAL FILTER TABLE	/////////////////////////////////////////////////////////////////

// Defining Type of Operations
typedef enum type_morphing {
	SHRINKING = 0x1,
	THINNING  = 0x2,
	SKELETONIZING = 0x3
} type_morphing;

//Returns bond of an Input 3x3 Window or Mask
int find_bond_mask(uint8_t mask);

//Number of Mask for a given bond value - Stage 1
int get_num_elements_for_stage1_bond(int bond, type_morphing type);

//Number of Masks for a given bond value - Stage 1
int get_num_elements_for_stage2_bond(int bond, type_morphing type);

//Returns a Mask based on bond value, index and operation type (Shrinking, Thinning, Skeletonizing) - Stage 1
uint8_t get_stage1_mask_for_bond(int bond, int index, type_morphing type);

//Returns a Mask based on bond value, index and operation type (Shrinking, Thinning, Skeletonizing) - Stage 1
uint8_t get_stage2_mask_for_bond(int bond, int index, type_morphing type);

//Prints the Mask, for Debugging purpose
void print_mask(uint8_t mask); 

//Prints the whole table - Shrinking, Thinning, Skeletonizing
void print_mask_table();

#endif