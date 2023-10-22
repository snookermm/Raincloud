#ifndef _INT_CTL_H_
#define _INT_CTL_H_
enum INT_TYPE
{
	INT,
	TRAP
};

#define INT_VECTOR_NUM		256

int int_init(void);


#endif
