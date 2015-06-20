#include "DryEngine/Engine.h"
#include "IntroState.h"

int main (void){
	drInit ();
	drChangeState (&IntroState);
	int exitstatus = drMainLoop ();
	drCleanup ();
	return exitstatus;
}