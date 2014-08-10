#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "common.h"
#include "datatypes.h"
#include "heuristics.h"
#include "move.h"
#include "piece.h"

const int maxdepth = MAX_PLY_DEPTH + MAX_EXTRA_PLY_DEPTH;
	

void Heuristics_produced_cutoff(unsigned int killers[], int depth, Move *m) {
	unsigned int num = Move_get_as_int(m);
	//printf(" = %d", num);
	if (killers[depth] != num) {
		//printf(", new killer move on level %d", depth);
		killers[depth] = num;
	}
}

void Heuristics_reorder(unsigned int killers[], int depth, Move **head) {
	Move *curr = *head;
	Move *prev = NULL;
	int i = 0;
	while (curr != NULL) {
		unsigned int num = Move_get_as_int(curr);
		if (killers[depth] == num) {
			if (i == 0) {
				// Nothing to do
				return;
			}

			// Move to the front!
			//printf(", moving ");
			//Move_print(curr);
			//printf(" (=%d) to the front from spot %d...", num, i);

			Move *next = curr->next_sibling;	// backup next in line
			curr->next_sibling = *head;			// head comes 2nd
			*head = curr;						// current becomes the new head
			if (prev != NULL) {					// glue the gap that's left behind
				prev->next_sibling = next;
			}
			return;
		}
		// Moving on!
		prev = curr;
		curr = curr->next_sibling;
		i++;
	}
	//printf(" no killer moves on level %d", depth);
}