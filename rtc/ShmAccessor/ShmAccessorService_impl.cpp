// -*- mode: c++; indent-tabs-mode: t; tab-width: 4; c-basic-offset: 4; -*-
#include <iostream>
#include "ShmAccessorService_impl.h"

ShmAccessorService_impl::ShmAccessorService_impl()
{
}

ShmAccessorService_impl::~ShmAccessorService_impl()
{
}

void ShmAccessorService_impl::echo(const char *msg)
{
	std::cout << "ShmAccessorService: " << msg << std::endl;
}

