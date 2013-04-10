/* The source code for this project loosely follows the Quake 2 code style. */

typedef struct SomeStruct_s 
{
 	int myVar;
	int (*someFP)(int var);
} SomeType_t;

static unsigned int SUBSYSTEM_SomeFunction(int *somePointer);

int g_SUBSYSTEMSomeGlobal; //defined as extern in header

static char m_memberVariable;

/* We can describe what the function does here if necessary */
static unsigned int 
SUBSYSTEM_SomeFunction(int *somePointer)
{
	int someLocal = *somePointer;
	
	if (somePointer == 0)
	{
		SUBSYSTEM_DoSomething(SUBSYSTEM_SOMETHING_IS_BROKEN);
	}
	else
	{
		puts("Not to worry!");
	}
	
	return someLocal;
}
