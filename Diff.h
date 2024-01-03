#ifndef _ROS_tortoisebot_mini_Diff_h
#define _ROS_tortoisebot_mini_Diff_h

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "ros/msg.h"
#include "std_msgs/Bool.h"
#include "std_msgs/UInt8.h"

namespace tortoisebot_mini
{

  class Diff : public ros::Msg
  {
    public:
      typedef std_msgs::Bool _ldir_type;
      _ldir_type ldir;
      typedef std_msgs::UInt8 _lrpm_type;
      _lrpm_type lrpm;
      typedef std_msgs::Bool _rdir_type;
      _rdir_type rdir;
      typedef std_msgs::UInt8 _rrpm_type;
      _rrpm_type rrpm;

    Diff():
      ldir(),
      lrpm(),
      rdir(),
      rrpm()
    {
    }

    virtual int serialize(unsigned char *outbuffer) const override
    {
      int offset = 0;
      offset += this->ldir.serialize(outbuffer + offset);
      offset += this->lrpm.serialize(outbuffer + offset);
      offset += this->rdir.serialize(outbuffer + offset);
      offset += this->rrpm.serialize(outbuffer + offset);
      return offset;
    }

    virtual int deserialize(unsigned char *inbuffer) override
    {
      int offset = 0;
      offset += this->ldir.deserialize(inbuffer + offset);
      offset += this->lrpm.deserialize(inbuffer + offset);
      offset += this->rdir.deserialize(inbuffer + offset);
      offset += this->rrpm.deserialize(inbuffer + offset);
     return offset;
    }

    virtual const char * getType() override { return "tortoisebot_mini/Diff"; };
    virtual const char * getMD5() override { return "17139eb8b7396bd528f221202cbcec9c"; };

  };

}
#endif
