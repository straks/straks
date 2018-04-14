// Copyright (c) 2017-2018 STRAKS developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef STRAKS_ZMQ_ZMQCONFIG_H
#define STRAKS_ZMQ_ZMQCONFIG_H

#if defined(HAVE_CONFIG_H)
#include "config/straks-config.h"
#endif

#include <stdarg.h>
#include <string>

#if ENABLE_ZMQ
#include <zmq.h>
#endif

#include "primitives/block.h"
#include "primitives/transaction.h"

void zmqError(const char *str);

#endif // STRAKS_ZMQ_ZMQCONFIG_H
