/*
 * Copyright 2009-2012 Scott McCreary
 * Copyright 2014 Puck Meerburg
 * Based on BeVexed by DarkWyrm Copyright 2007-2009
 *
 * Distributed under terms of the MIT License.
 *
 */
#ifndef HEXTILE_H
#define HEXTILE_H

#include <Archivable.h>

class HexTile : public BArchivable
{
public:
	// Archiving stuff!
	HexTile(BMessage *from);
	static BArchivable *Instantiate(BMessage *from);

	virtual ~HexTile() {};
	virtual status_t AllArchived(BMessage *archive) const;
	virtual status_t AllUnarchived(const BMessage *archive);
	virtual status_t Archive(BMessage *into, bool deep = true);
	virtual status_t Perform(perform_code d, void *arg) { return B_ERROR; }
	
	// Normal stuff!
	HexTile(void);
	HexTile(const uint8 &tl, const uint8 &t, const uint8 &tr, 
			const uint8 &bl, const uint8 &b, const uint8 &br);
	HexTile(const HexTile &t);
	HexTile &operator=(const HexTile &t);
	
	void SetValues( const uint8 &tl, const uint8 &t, const uint8 &tr, 
					const uint8 &bl, const uint8 &b, const uint8 &br);
	
	bool IsEmpty(void) const { return (topleft<0 || top<0 || topright<0 || bottomleft<0 || bottom<0 || bottomright<0); }
	void MakeEmpty(void) { topleft=top=topright=bottomleft=bottom=bottomright=-1; }
	
	int8 topleft;
	int8 top;
	int8 topright;
	int8 bottomleft;
	int8 bottom;
	int8 bottomright;
	uint16 id;
	
	HexTile *toplefttile,*toptile,*toprighttile,*bottomlefttile,*bottomtile,*bottomrighttile;
};

#endif
