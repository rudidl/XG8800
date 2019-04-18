/*
 XG8800 hardware header for www.x-graph.be
 Copyright (c) 2016 DELCOMp bvba / UVee bvba
 2sd (a t) delcomp (d o t) com

 License:GNU General Public License v3.0
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _XG8800_H_
#define _XG8800_H_

#if !defined(SPARK) && defined(STM32F2XX)
#if defined(ARDUINO)
    SYSTEM_MODE(SEMI_AUTOMATIC);
#endif
#endif

#include <XGLCD.h>

#endif
