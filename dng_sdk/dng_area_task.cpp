/*****************************************************************************/
// Copyright 2006-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#include "dng_area_task.h"

#include "dng_abort_sniffer.h"
#include "dng_auto_ptr.h"
#include "dng_flags.h"
#include "dng_globals.h"
#include "dng_sdk_limits.h"
#include "dng_tile_iterator.h"
#include "dng_utils.h"

/*****************************************************************************/

dng_area_task::dng_area_task (const char *name)

	:	fMaxThreads   (kMaxMPThreads)
	
	,	fMinTaskArea  (256 * 256)
	
	,	fUnitCell	  (1, 1)
	
	,	fMaxTileSize  (256, 256)

	,	fName ()

	{

	if (!name)
		{
		name = "dng_area_task";
		}

	fName.Set (name);
	
	}

/*****************************************************************************/

dng_area_task::~dng_area_task ()
	{
	
	}

/*****************************************************************************/

dng_rect dng_area_task::RepeatingTile1 () const
	{
	
	return dng_rect ();
	
	}
		
/*****************************************************************************/

dng_rect dng_area_task::RepeatingTile2 () const
	{
	
	return dng_rect ();
	
	}
		
/*****************************************************************************/

dng_rect dng_area_task::RepeatingTile3 () const
	{
	
	return dng_rect ();
	
	}
		
/*****************************************************************************/

void dng_area_task::Start (uint32 /* threadCount */,
						   const dng_rect & /* dstArea */,
						   const dng_point & /* tileSize */,
						   dng_memory_allocator * /* allocator */,
						   dng_abort_sniffer * /* sniffer */)
	{
	
	}

/*****************************************************************************/

void dng_area_task::Finish (uint32 /* threadCount */)
	{
	
	}
	
/*****************************************************************************/

dng_point dng_area_task::FindTileSize (const dng_rect &area) const
	{
	
	dng_rect repeatingTile1 = RepeatingTile1 ();
	dng_rect repeatingTile2 = RepeatingTile2 ();
	dng_rect repeatingTile3 = RepeatingTile3 ();
	
	if (repeatingTile1.IsEmpty ())
		{
		repeatingTile1 = area;
		}
	
	if (repeatingTile2.IsEmpty ())
		{
		repeatingTile2 = area;
		}
	
	if (repeatingTile3.IsEmpty ())
		{
		repeatingTile3 = area;
		}
		
	uint32 repeatV = Min_uint32 (Min_uint32 (repeatingTile1.H (),
											 repeatingTile2.H ()),
											 repeatingTile3.H ());
	
	uint32 repeatH = Min_uint32 (Min_uint32 (repeatingTile1.W (),
											 repeatingTile2.W ()),
											 repeatingTile3.W ());
	
	dng_point maxTileSize = MaxTileSize ();

	dng_point tileSize;
		
	tileSize.v = Min_int32 (repeatV, maxTileSize.v);
	tileSize.h = Min_int32 (repeatH, maxTileSize.h);
	
	// Make Xcode happy (div by zero).

	tileSize.v = Max_int32 (tileSize.v, 1);
	tileSize.h = Max_int32 (tileSize.h, 1);

	// What this is doing is, if the smallest repeating image tile is larger than the 
	// maximum tile size, adjusting the tile size down so that the tiles are as small
	// as possible while still having the same number of tiles covering the
	// repeat area.  This makes the areas more equal in size, making MP
	// algorithms work better.
						
	// The image core team did not understand this code, and disabled it.
	// Really stupid idea to turn off code you don't understand!
	// I'm undoing this removal, because I think the code is correct and useful.

	uint32 countV = (repeatV + tileSize.v - 1) / tileSize.v;
	uint32 countH = (repeatH + tileSize.h - 1) / tileSize.h;

	// Make Xcode happy (div by zero).

	countV = Max_uint32 (countV, 1);
	countH = Max_uint32 (countH, 1);
	
	tileSize.v = (repeatV + countV - 1) / countV;
	tileSize.h = (repeatH + countH - 1) / countH;
	
	// Round up to unit cell size.
	
	dng_point unitCell = UnitCell ();
	
	if (unitCell.h != 1 || unitCell.v != 1)
		{
		tileSize.v = ((tileSize.v + unitCell.v - 1) / unitCell.v) * unitCell.v;
		tileSize.h = ((tileSize.h + unitCell.h - 1) / unitCell.h) * unitCell.h;
		}
		
	// But if that is larger than maximum tile size, round down to unit cell size.
	
	if (tileSize.v > maxTileSize.v)
		{
		tileSize.v = (maxTileSize.v / unitCell.v) * unitCell.v;
		}

	if (tileSize.h > maxTileSize.h)
		{
		tileSize.h = (maxTileSize.h / unitCell.h) * unitCell.h;
		}
		
    if (gPrintTimings)
		{
        fprintf (stdout,
                 "\nRender tile for below: %d x %d\n",
                 (int32) tileSize.h,
                 (int32) tileSize.v);
		}	

	return tileSize;
	
	}
		
/*****************************************************************************/

void dng_area_task::ProcessOnThread (uint32 threadIndex,
									 const dng_rect &area,
									 const dng_point &tileSize,
									 dng_abort_sniffer *sniffer,
                                     dng_area_task_progress *progress)
	{

	dng_rect repeatingTile1 = RepeatingTile1 ();
	dng_rect repeatingTile2 = RepeatingTile2 ();
	dng_rect repeatingTile3 = RepeatingTile3 ();
	
	if (repeatingTile1.IsEmpty ())
		{
		repeatingTile1 = area;
		}
	
	if (repeatingTile2.IsEmpty ())
		{
		repeatingTile2 = area;
		}
	
	if (repeatingTile3.IsEmpty ())
		{
		repeatingTile3 = area;
		}
		
	dng_rect tile1;

	// TODO_EP: Review & document case where these dynamic allocations appeared to have significant overhead
	AutoPtr<dng_base_tile_iterator> iter1
		(MakeTileIterator (threadIndex,
						   repeatingTile3, 
						   area));
	
	while (iter1->GetOneTile (tile1))
		{
		
		dng_rect tile2;
		
		AutoPtr<dng_base_tile_iterator> iter2
			(MakeTileIterator (threadIndex,
							   repeatingTile2, 
							   tile1));
	
		while (iter2->GetOneTile (tile2))
			{
			
			dng_rect tile3;
			
			AutoPtr<dng_base_tile_iterator> iter3
				(MakeTileIterator (threadIndex,
								   repeatingTile1, 
								   tile2));
			
			while (iter3->GetOneTile (tile3))
				{
				
				dng_rect tile4;
				
				AutoPtr<dng_base_tile_iterator> iter4
					(MakeTileIterator (threadIndex,
									   tileSize, 
									   tile3));
				
				while (iter4->GetOneTile (tile4))
					{
					
					dng_abort_sniffer::SniffForAbort (sniffer);
					
					Process (threadIndex, tile4, sniffer);

                    if (progress)
                        {
                        progress->FinishedTile (tile4);
                        }
					
					}
					
				}
				
			}
		
		}

	}

/*****************************************************************************/

dng_base_tile_iterator * dng_area_task::MakeTileIterator (uint32 /* threadIndex */,
														  const dng_rect &tile,
														  const dng_rect &area) const
	{
	
	return new dng_tile_forward_iterator (tile, area);
	
	}
		
/*****************************************************************************/

dng_base_tile_iterator * dng_area_task::MakeTileIterator (uint32 /* threadIndex */,
														  const dng_point &tileSize,
														  const dng_rect &area) const
	{
	
	return new dng_tile_forward_iterator (tileSize, area);
	
	}
		
/*****************************************************************************/

void dng_area_task::Perform (dng_area_task &task,
				  			 const dng_rect &area,
				  			 dng_memory_allocator *allocator,
				  			 dng_abort_sniffer *sniffer,
                             dng_area_task_progress *progress)
	{
	
	dng_point tileSize (task.FindTileSize (area));
		
	task.Start (1, area, tileSize, allocator, sniffer);
	
	task.ProcessOnThread (0, area, tileSize, sniffer, progress);
			
	task.Finish (1);
	
	}

/*****************************************************************************/
