#ifndef ONLINE_TRAJECTORY_MODIFICATION_SHM_H
#define ONLINE_TRAJECTORY_MODIFICATION_SHM_H

#define MAX_JOINT_NUM 64
#define OTM_STEP_COUNT 25 // 50ms / 2ms

enum OTMstate {SA_DISABLE, SA_ENABLE, EUS_TRIGGER, EUS_ACCESSIBLE, EUS_DISABLE};

struct otm_shm {
  short otm_flag;
  float ref_angle_buf[OTM_STEP_COUNT][MAX_JOINT_NUM];
  enum OTMstate otm_state;
};


#endif
