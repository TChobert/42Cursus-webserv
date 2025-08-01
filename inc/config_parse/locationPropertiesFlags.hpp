#pragma once

struct locationPropertiesFlags {

	bool allowedMethodsSeen;
	bool uploadAuthSeen;
	bool uploadLocSeen;
	bool autoIndexSeen;
	bool indexSeen;
	bool cgiSeen;
	bool returnSeen;

	locationPropertiesFlags(void);
	~locationPropertiesFlags(void);
	void resetFlags(void);
};
