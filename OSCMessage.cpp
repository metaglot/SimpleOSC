#include "stdafx.h"
#include "OSCMessage.h"

std::string OSCMessage::getHost() 
{
	return host;
}

void OSCMessage::setHost(std::string h) 
{
	host = h;
}

OSCAddress OSCMessage::prependAddress(OSCAddress pre) 
{
	rendered = false;
	return (oscaddress = pre + oscaddress);
}

OSCAddress OSCMessage::appendAddress(OSCAddress post) 
{
	rendered = false;
	return (oscaddress += post);
}

OSCAddress OSCMessage::setAddress(OSCAddress osc_address) 
{
	rendered = false;
	return (oscaddress = osc_address);
}

OSCAddress OSCMessage::getAddress()
{
	return oscaddress;
}

void OSCMessage::addArgument(OSCFloat f) 
{
	oscarg o;
	o.typechar = 'f';
	o.data_float = f;
	oscargs.push_back(o);
	rendered = false;
}

void OSCMessage::addArgument(OSCInt i) 
{
	oscarg o;
	o.typechar = 'i';
	o.data_int = i;
	oscargs.push_back(o);
	rendered = false;
}

void OSCMessage::addArgument(OSCString s)
{
	oscarg o;
	o.typechar = 's';
	o.data_string = s;
	oscargs.push_back(o);
	rendered = false;
}

void OSCMessage::addf(OSCFloat f) 
{
	addArgument(f);
}

void OSCMessage::addi(OSCInt i) 
{
	addArgument(i);
}

void OSCMessage::adds(OSCString s)
{
	addArgument(s);
}

OSCFloat OSCMessage::getf(unsigned int index) 
{
	return oscargs[index].data_float;
}

OSCInt OSCMessage::geti(unsigned int index) 
{
	return oscargs[index].data_int;
}

OSCString OSCMessage::gets(unsigned int index) 
{
	return oscargs[index].data_string;
}

void OSCMessage::clearargs() 
{
	oscargs.clear();
}

unsigned int OSCMessage::argsize() 
{
	return oscargs.size();
}

char OSCMessage::argtype(unsigned int index) 
{ 
	return oscargs[index].typechar; 
}

std::vector<char> OSCMessage::formattedByteArray()
{
	if (rendered)
	{
		return cache;
	}

	//std::vector<char> cache;
	cache.clear();
	unsigned int i = 0;

	auto chr = [&](char c) {
		cache.push_back(c);
		i++;
	};

	auto padosc = [&]() {
		while ((i & 3) != 0)
		{
			chr(0);
		}
	};
	for (unsigned int k = 0; k < oscaddress.length(); k++)
	{
		chr(oscaddress[k]);
	}
	chr(0);
	padosc();

	//arguments
	if (oscargs.size() != 0)
	{
		chr(',');
		for (auto j = oscargs.begin(); j != oscargs.end(); j++)
		{
			chr(j->typechar);
		}
		chr(0);
		padosc();
		for (auto j = oscargs.begin(); j != oscargs.end(); j++)
		{
			if (j->typechar == 'i')
			{
				fourcharint bi;
				bi.integ = j->data_int;
				//vchar
				chr(bi.chr[3]);
				chr(bi.chr[2]);
				chr(bi.chr[1]);
				chr(bi.chr[0]);
			}
			else if (j->typechar == 'f')
			{
				fourcharfloat bf;
				bf.flo = j->data_float;
				chr(bf.chr[3]);
				chr(bf.chr[2]);
				chr(bf.chr[1]);
				chr(bf.chr[0]);
			}
			else if (j->typechar == 's')
			{
				for (unsigned int i = 0; i < j->data_string.size(); i++)
				{
					chr(j->data_string[i]);
				}
				chr(0);
				padosc();
			}
		}
		chr(0);
		padosc();
	}
	//cache = cache;
	rendered = true;
	return cache;
}

void OSCMessage::render()
{
	formattedByteArray();
}

OSCMessage& OSCMessage::parseOSCMessage(char incoming[], unsigned int length) {
	using namespace std;
	OSCMessage & omsg = *this;
	if (length == 0) return omsg;
	if ((length & 3) == 0)
	{
		//unsigned int address_length = 0; //length 
		unsigned int tts_offset = 0;
		unsigned int arg_offset = 0;
		unsigned int gifa_argcount = -1;

		OSCAddress osc_address;
		auto zeroedge = [&](unsigned int t) {
			while (incoming[--t] == 0 && t < length)
				;
			return t + 1;
		};

		auto getintfloatarg = [&](char type, unsigned int argoffset) {
			unsigned int argofs = argoffset + (4 * ++gifa_argcount);
			if (type == 'f')
			{
				fourcharfloat fcf;
				fcf.chr[0] = incoming[argofs + 3];
				fcf.chr[1] = incoming[argofs + 2];
				fcf.chr[2] = incoming[argofs + 1];
				fcf.chr[3] = incoming[argofs];
				omsg.addf(fcf.flo);
			}
			else if (type == 'i')
			{
				fourcharint fci;
				fci.chr[0] = incoming[argofs + 3];
				fci.chr[1] = incoming[argofs + 2];
				fci.chr[2] = incoming[argofs + 1];
				fci.chr[3] = incoming[argofs];
				omsg.addi(fci.integ);
			}
			else if (type == 's')
			{
				char * bptr = &incoming[argofs];
				//unsigned int flen = argofs + 1;
				while (incoming[argofs + 3] != 0 && argofs < length)
				{
					argofs = argoffset + (4 * ++gifa_argcount);
				}

				omsg.adds(OSCString(bptr));
			}
			else if (type == 'b')
			{
				fourcharint fci;
				fci.chr[0] = incoming[argofs + 3];
				fci.chr[1] = incoming[argofs + 2];
				fci.chr[2] = incoming[argofs + 1];
				fci.chr[3] = incoming[argofs];
				gifa_argcount += fci.integ / 4;
				//read int32 size count at argofs + [0 - 3]
				//offset argofs and move on
				//omsg.oscargs.push_back(o); //ignores binary for now
			}
		};

		for (unsigned int i = 0; i < (length / 4); i++)
		{
			unsigned int ir = i * 4;
			if (incoming[ir + 3] == 0 && tts_offset == 0)
			{
				tts_offset = ir + 4;
				unsigned int adr_length = zeroedge(tts_offset - 1);
				omsg.setAddress(OSCAddress(&incoming[0], adr_length));
				continue;
			}
			else if (incoming[ir + 3] == 0 && arg_offset == 0 && tts_offset != 0)
			{
				arg_offset = ir + 4;
				break;
			}
		}
		unsigned int j = 0;
		for (unsigned int i = tts_offset; i < arg_offset; i++)
		{
			j++;
			if (!incoming[i]) break;
			if (i == 0 && incoming[i] != ',')
			{
				//somethings afoot
			}
			else if (i > 0)
			{
				oscarg oa;
				switch (incoming[i])
				{
				case 'i':
					getintfloatarg('i', arg_offset);
					break;
				case 'f':
					getintfloatarg('f', arg_offset);
					break;
				case 's':
					getintfloatarg('s', arg_offset);
					break;
				case 'b':
					getintfloatarg('b', arg_offset);
					break;
				default:
					//confused
					//panic?
					break;
				}
			}
		}
	}
	else
	{
		//process in other ways
	}
	return omsg;
}