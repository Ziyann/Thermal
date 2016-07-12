#ifndef CONTROL_H_
#define CONTROL_H_

#define CMD_SET_HORIZ_ANGLE		'h'
#define CMD_SET_VERT_ANGLE		'v'
#define CMD_RESET_SERVOS		'r'

#define CMD_GET_AMB_TEMP		'a'
#define CMD_GET_OBJ_TEMP		'o'
#define CMD_GET_IDENT			'i'

void parse_command(char*);

#endif /* CONTROL_H_ */