// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*-
#ifndef __SHM_ACCESSOR_SERVICE_H__
#define __SHM_ACCESSOR_SERVICE_H__

#include "hrpsys/idl/ShmAccessorService.hh"

class ShmAccessorService_impl
	: public virtual POA_OpenHRP::ShmAccessorService,
	  public virtual PortableServer::RefCountServantBase
{
public:
	/**
	   \brief constructor
	*/
	ShmAccessorService_impl();

	/**
	   \brief destructor
	*/
	virtual ~ShmAccessorService_impl();

	void echo(const char *msg);
private:
};

#endif
