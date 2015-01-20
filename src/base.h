#ifndef RTOS_BASE_H_
#define RTOS_BASE_H_

#include <stdlib.h>

class _class
{
	public:

		void *operator new (size_t size)
		{
			return malloc (size);
		}

		void operator delete (void *ptr)
		{
			free (ptr);
		}
};

#endif /* RTOS_BASE_H_ */
