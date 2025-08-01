#pragma once

struct locationPropertiesFlags {

	bool allowedMethodsSeen;
	bool uploadAuthSeen;
	bool uploadLocSeen;
	bool autoIndexSeen;
	bool indexSeen;

	locationPropertiesFlags(void);
	~locationPropertiesFlags(void);
	void resetFlags(void);
};
