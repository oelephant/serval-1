#include "include.h"

#include "object.h"
#include "check.h"

struct Check check;

void check_addItem(struct Food *food){
    uint8_t i;
    if (check.length > MAX_CHECK_LENGTH){
	return;
    }

    for (i = 0; i < check.length; i++){
	if (check.foods[i]->id == food->id){
	    check.qty[i]++;
	    return;
	}
    }
    check.foods[i] = food;
    check.qty[i]++;
    check.length++;
}

void check_removeItem(struct Food *food){

}