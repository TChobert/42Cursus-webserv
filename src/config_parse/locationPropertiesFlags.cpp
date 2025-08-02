#include "locationPropertiesFlags.hpp"

locationPropertiesFlags::locationPropertiesFlags(void) : allowedMethodsSeen(false) {}

locationPropertiesFlags::~locationPropertiesFlags(void) {}

void locationPropertiesFlags::resetFlags(void) {

	allowedMethodsSeen = false;
	uploadAuthSeen = false;
	uploadDirSeen = false;
	autoIndexSeen = false;
	indexSeen = false;
	cgiSeen = false;
	returnSeen = false;
	maxBodySeen = false;
}
