#include "locationPropertiesFlags.hpp"

locationPropertiesFlags::locationPropertiesFlags(void)
	: rootSeen(false),
	allowedMethodsSeen(false),
	uploadAuthSeen(false),
	uploadDirSeen(false), 
	autoIndexSeen(false),
	indexSeen(false),
	cgiSeen(false),
	returnSeen(false),
	maxBodySeen(false) {}

locationPropertiesFlags::~locationPropertiesFlags(void) {}

void locationPropertiesFlags::resetFlags(void) {

	rootSeen = false;
	allowedMethodsSeen = false;
	uploadAuthSeen = false;
	uploadDirSeen = false;
	autoIndexSeen = false;
	indexSeen = false;
	cgiSeen = false;
	returnSeen = false;
	maxBodySeen = false;
}
