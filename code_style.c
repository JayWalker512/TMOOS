/* The source code for this project loosely follows the Quake 2 code style. */

static unsigned int SUBSYSTEM_SomeFunction(int *somePointer);

int g_SUBSYSTEMSomeGlobal; //defined as extern in header

static char m_memberVariable;

/* We can describe what the function does here if necessary */
static unsigned int 
SUBSYSTEM_SomeFunction(int *somePointer)
{
	int someLocal = &somePointer;
	
	if (someLocal == 0)
	{
		SUBSYSTEM_DoSomething(SUBSYSTEM_SOMETHING_IS_BROKEN);
	}
	else
	{
		puts("Not to worry!");
	}
	
	return someLocal;
}
