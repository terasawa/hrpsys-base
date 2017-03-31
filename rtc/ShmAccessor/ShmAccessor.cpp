// -*- C++ -*-
/*!
 * @file  ShmAccessor.cpp
 * @brief null component
 * $Date$
 *
 * $Id$
 */

#include "hrpsys/util/VectorConvert.h"
#include "shm_common.h"
#include "ShmAccessor.h"
#include <rtm/CorbaNaming.h>
#include <hrpModel/ModelLoaderUtil.h>

// Module specification
// <rtc-template block="module_spec">
static const char* shmaccessor_spec[] =
  {
    "implementation_id", "ShmAccessor",
    "type_name",         "ShmAccessor",
    "description",       "access shared memory",
    "version",           HRPSYS_PACKAGE_VERSION,
    "vendor",            "AIST",
    "category",          "example",
    "activity_type",     "DataFlowComponent",
    "max_instance",      "10",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.string", "test",
    "conf.default.intvec", "1,2,3",
    "conf.default.double", "1.234",

    ""
  };
// </rtc-template>

ShmAccessor::ShmAccessor(RTC::Manager* manager)
  : RTC::DataFlowComponentBase(manager),
    // <rtc-template block="initializer">
    m_dataIn("qRef", m_data),
    m_dataOut("q", m_data)
    // m_ShmAccessorServicePort("ShmAccessorService"),
    // </rtc-template>
{
  std::cout << "ShmAccessor::ShmAccessor()" << std::endl;
  // m_data.data = 0;
}

ShmAccessor::~ShmAccessor()
{
  std::cout << "ShmAccessor::~ShmAccessor()" << std::endl;
}



RTC::ReturnCode_t ShmAccessor::onInitialize()
{
  std::cout << m_profile.instance_name << ": onInitialize()" << std::endl;
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  
  // </rtc-template>

  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("qRef", m_dataIn);

  // Set OutPort buffer
  addOutPort("q", m_dataOut);
  
  // Set service provider to Ports
  // m_ShmAccessorServicePort.registerProvider("service0", "ShmAccessorService", m_ShmAccessorService);
  
  // set service consumers to Ports
  
  // Set CORBA Service Ports
  // addPort(m_ShmAccessorServicePort);
  
  // </rtc-template>

  RTC::Properties& prop = getProperties();
  std::cout << "prop[\"testconf\"] = " << prop["testconf"] << std::endl;

  // initialize robot model
  m_robot = hrp::BodyPtr(new hrp::Body());
  RTC::Manager& rtcManager = RTC::Manager::instance();
  std::string nameServer = rtcManager.getConfig()["corba.nameservers"];
  int comPos = nameServer.find(",");
  if (comPos < 0) {
    comPos = nameServer.length();
  }
  nameServer = nameServer.substr(0, comPos);
  RTC::CorbaNaming naming(rtcManager.getORB(), nameServer.c_str());
  if (!loadBodyFromModelLoader(m_robot, prop["model"].c_str(),
                               CosNaming::NamingContext::_duplicate(naming.getRootContext())
        )){
    std::cerr << "[" << m_profile.instance_name << "] failed to load model[" << prop["model"] << "]"
              << std::endl;
  }
  
  
  // initialize shared memory
  m_s_shm = (struct otm_shm *)set_shared_memory(7777, sizeof(struct otm_shm));
  period_counter = OTM_STEP_COUNT;
  m_s_shm->otm_flag = SA_DISABLE;

  // initialize dataport
  m_data.data.length(m_robot->numJoints());

  return RTC::RTC_OK;
}



/*
RTC::ReturnCode_t ShmAccessor::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ShmAccessor::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ShmAccessor::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t ShmAccessor::onActivated(RTC::UniqueId ec_id)
{
  std::cout << "[" << m_profile.instance_name<< "] onActivated(" << ec_id << ")" << std::endl;
  return RTC::RTC_OK;
}

RTC::ReturnCode_t ShmAccessor::onDeactivated(RTC::UniqueId ec_id)
{
  std::cout << "[" << m_profile.instance_name<< "] onDeactivated(" << ec_id << ")" << std::endl;
  return RTC::RTC_OK;
}

RTC::ReturnCode_t ShmAccessor::onExecute(RTC::UniqueId ec_id)
{
  // std::cout << m_profile.instance_name<< ": onExecute(" << ec_id << "), data = " << m_data.data << std::endl;
  // std::cout << m_profile.instance_name<< ": onExecute(" << ec_id << ")" << std::endl;

  // read input port
  while (m_dataIn.isNew()){
      m_dataIn.read();
      // std::cout << m_profile.instance_name << ": read(), data = " << m_data.data << std::endl;
  }

  // read ref_angle from shm
  int i, n;
  n = m_robot->numJoints();
  if (n > MAX_JOINT_NUM) {
      n = MAX_JOINT_NUM;
  }

  std::vector<double> ref_q(n, 0.0);
  for (i = 0; i < n; ++i) {
      ref_q[i] = m_data.data[i];
  }

  std::cout << "cycle = " << period_counter << std::endl;
  std::cout << "m_s_shm->otm_flag = " << m_s_shm->otm_flag << std::endl;

  if (m_s_shm->otm_flag != SA_DISABLE) {
      switch (m_s_shm->otm_flag) {
      case SA_ENABLE:
          period_counter = OTM_STEP_COUNT;
          m_s_shm->otm_flag = EUS_TRIGGER;
          // std::cout << "m_s_shm->otm_flag = EUS_TRIGGER!! : " << m_s_shm->otm_flag << std::endl;
          break;
      case EUS_ACCESSIBLE:
          if (--period_counter <= 0) {
              period_counter = OTM_STEP_COUNT;
              std::cout << "m_s_shm->otm_flag = " << m_s_shm->otm_flag << std::endl;
              for (i = 0; i < OTM_STEP_COUNT; ++i) {
                  std::vector<double> tmp_ref_angle(n, 0.0);
                  for (int j = 0; j < n; ++j) {
                      tmp_ref_angle[j] = m_s_shm->ref_angle_buf[i][j];
                  }
                  ref_angle_vector_buf.push_back(tmp_ref_angle);
              }
              // CAUTION!! you should adjust joint angles if you change flag 0 from eus
              // m_s_shm->otm_flag = 0; // false
              m_s_shm->otm_flag = EUS_TRIGGER;
          }
          break;
      case EUS_TRIGGER:
          --period_counter;
          break;
      case EUS_DISABLE:
          period_counter = OTM_STEP_COUNT;
          break;
      default:
          break;
      }

      if (!ref_angle_vector_buf.empty()) {
          for (i = 0; i < n; ++i) {
              ref_q[i] = ref_angle_vector_buf.front()[i];
          }
          ref_angle_vector_buf.erase(ref_angle_vector_buf.begin());
      }
  } else {
      period_counter = OTM_STEP_COUNT;
  }


  for (i = 0; i < n; ++i) {
      m_data.data[i] = ref_q[i];
  }


  // output ref_angle from shm
  m_dataOut.write();
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ShmAccessor::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ShmAccessor::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ShmAccessor::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ShmAccessor::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ShmAccessor::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

extern "C"
{

  void ShmAccessorInit(RTC::Manager* manager)
  {
    RTC::Properties profile(shmaccessor_spec);
    manager->registerFactory(profile,
                             RTC::Create<ShmAccessor>,
                             RTC::Delete<ShmAccessor>);
  }

};

