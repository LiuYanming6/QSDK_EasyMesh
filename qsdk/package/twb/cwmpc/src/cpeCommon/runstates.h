/*
 * runstates.h
 *
 *  Created on: Jan 11, 2012
 *      Author: dmounday
 */

#ifndef RUNSTATES_H_
#define RUNSTATES_H_
/*
 * cwmpRunState CWMPc cpe state values. All state value set by the
 * framework are negative.
 * The CPE should use positive values. The following are defined in the
 * quick-start code. Additional state value may be added if required.
 */
#define RUNSTATE_CPEINIT	2
#define RUNSTATE_RESTORE	3
#define	RUNSTATE_RESTORE_OBJ	4
#define	RUNSTATE_RESTORE_PARAM	5
#define	RUNSTATE_SAVECONFIG		6

#endif /* RUNSTATES_H_ */
