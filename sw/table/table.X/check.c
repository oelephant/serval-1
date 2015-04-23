#include "include.h"

#include "object.h"
#include "check.h"

struct Check check;

void check_addItem(struct Food *food){
    unsigned int i;
    if (check.length > MAX_CHECK_LENGTH){
	return;
    }

    for (i = 0; i < check.length; i++){
	if (check.foods[i]->id == food->id){
	    check.qty[i]++;
	    check.totalPrice += check.foods[i]->price;
	    return;
	}
    }
    check.foods[i] = food;
    check.qty[i]++;
    check.totalPrice += check.foods[i]->price;
    check.length++;
}

void check_reset(void){
    check.length = 0;
    check.totalPrice = 0;
}

void check_removeItem(struct Food *food){
    unsigned int i,j;
    for (i = 0; i < check.length; i++){
	if (check.foods[i]->id == food->id){
	    if (check.qty[i] != 0){
		check.qty[i]--;
		check.totalPrice -= check.foods[i]->price;
		if (check.qty[i] == 0){
		    for (j = i; j < check.length; j++){
			check.foods[j] = check.foods[j+1];
			check.qty[j] = check.qty[j+1];
		    }
		    //check.foods[j] = NULL;
		    //check.qty[j] = 0;
		    check.length--;
		}
	    }
	    return;
	}
    }
}