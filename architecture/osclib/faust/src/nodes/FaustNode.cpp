/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#include "FaustNode.h"
#include "Message.h"
#include "OSCStream.h"

namespace oscfaust
{

//--------------------------------------------------------------------------
void FaustNode::store( float val )
{
	if (val > fMax) val = fMax;
	else if (val < fMin) val = fMin;
	*fZone = val;
}

//--------------------------------------------------------------------------
bool FaustNode::accept( const Message* msg )
{
	if (msg->size() == 1) {
		int ival; float fval;
		if (msg->param(0, fval)) store (fval);
		else if (msg->param(0, ival)) store (float(ival));	
		else return MessageDriven::accept(msg);
		return true;		return true;
	}
	return MessageDriven::accept(msg);
}


//--------------------------------------------------------------------------
void FaustNode::get (unsigned long ipdest ) const
{
	unsigned long savedip = oscout.getAddress();
	oscout.setAddress(ipdest);
	oscout << OSCStart(getOSCAddress().c_str()) << 	*fZone << fMin << fMax << OSCEnd();
	oscout.setAddress(savedip);
}

} // end namespoace