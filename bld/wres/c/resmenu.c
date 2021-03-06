/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <string.h>
#include "layer0.h"
#include "filefmt.h"
#include "write.h"
#include "read.h"
#include "resmenu.h"
#include "reserr.h"
#include "wresrtns.h"

bool ResWriteMenuHeader( MenuHeader *currhead, WResFileID handle )
/****************************************************************/
{
    if( WRESWRITE( handle, currhead, sizeof( MenuHeader ) ) != sizeof( MenuHeader ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool ResWriteMenuExHeader( MenuHeader *currhead, WResFileID handle, uint_8 *headerdata )
/**************************************************************************************/
{
    bool            error;
    int             i;

    if( WRESWRITE( handle, currhead, sizeof( MenuHeader ) ) != sizeof( MenuHeader ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        if( headerdata != NULL ) {
            for( i = 0; i < currhead->HeaderSize; i++ ) {
                error = ResWriteUint8( headerdata + i, handle );
                if( error ) {
                    WRES_ERROR( WRS_WRITE_FAILED );
                    return( true );
                }
            }
        }
        return( false );
    }
}

bool ResWriteMenuItemPopup( const MenuItemPopup *curritem, bool use_unicode, WResFileID handle )
/**********************************************************************************************/
{
    bool        error;
    uint_16     tmp16;

    if( curritem->ItemFlags & MENU_POPUP ) {
        tmp16 = curritem->ItemFlags;
        error = ResWriteUint16( &tmp16, handle );
        if( !error ) {
            error = ResWriteString( curritem->ItemText, use_unicode, handle );
        }
    } else {
        WRES_ERROR( WRS_BAD_PARAMETER );
        error = true;
    }

    return( error );
}

bool ResWriteMenuExItemPopup( const MenuItemPopup *curritem, const MenuExItemPopup *exdata,
                             bool use_unicode, WResFileID handle )
/*****************************************************************************/
{
    bool        error;
    uint_16     tmp16;

    if( curritem->ItemFlags & MENUEX_POPUP ) {
        error = ResWriteUint32( &(exdata->ItemType), handle );
        if( !error ) {
            error = ResWriteUint32( &(exdata->ItemState), handle );
        }
        if( !error ) {
            error = ResWriteUint32( &(exdata->ItemId), handle );
        }
        if( !error ) {
            tmp16 = curritem->ItemFlags;
            error = ResWriteUint16( &tmp16, handle );
        }
        if( !error ) {
            error = ResWriteString( curritem->ItemText, use_unicode, handle );
        }
        if( !error ) {
            error = ResPadDWord( handle );
        }
        if( !error ) {
            error = ResWriteUint32( &(exdata->HelpId), handle );
        }
    } else {
        WRES_ERROR( WRS_BAD_PARAMETER );
        error = true;
    }

    return( error );

}

bool ResWriteMenuItemNormal( const MenuItemNormal *curritem, bool use_unicode,
                                                    WResFileID handle )
/******************************************************************************/
{
    bool        error;
    uint_16     tmp16;

    if( curritem->ItemFlags & MENU_POPUP ) {
        WRES_ERROR( WRS_BAD_PARAMETER );
        error = true;
    } else {
        tmp16 = curritem->ItemFlags;
        error = ResWriteUint16( &tmp16, handle );
        if( !error ) {
            tmp16 = curritem->ItemID;
            error = ResWriteUint16( &tmp16, handle );
        }
        if( !error ) {
            error = ResWriteString( curritem->ItemText, use_unicode, handle );
        }
    }

    return( error );
}

bool ResWriteMenuExItemNormal( const MenuItemNormal *curritem, const MenuExItemNormal *exdata,
                              bool use_unicode, WResFileID handle )
/*******************************************************************************************/
{
    bool        error;
    uint_16     tmp16;
    uint_32     tmp32;

    if( curritem->ItemFlags & MENUEX_POPUP ) {
        WRES_ERROR( WRS_BAD_PARAMETER );
        error = true;
    } else {
        error = ResWriteUint32( &(exdata->ItemType), handle );
        if( !error ) {
            error = ResWriteUint32( &(exdata->ItemState), handle );
        }
        if( !error ) {
            tmp32 = curritem->ItemID;
            error = ResWriteUint32( &tmp32, handle );
        }
        if( !error ) {
            tmp16 = curritem->ItemFlags;
            error = ResWriteUint16( &tmp16, handle );
        }
        if( !error ) {
            error = ResWriteString( curritem->ItemText, use_unicode, handle );
        }
        if( !error ) {
            error = ResPadDWord( handle );
        }
    }

    return( error );
}

bool ResWriteMenuItem( const MenuItem *curritem, bool use_unicode,
                                             WResFileID handle )
/******************************************************************/
{
    bool    error;

    if( curritem->IsPopup ) {
        error = ResWriteMenuItemPopup( &(curritem->Item.Popup), use_unicode, handle );
    } else {
        error = ResWriteMenuItemNormal( &(curritem->Item.Normal), use_unicode, handle );
    }

    return( error );
}

bool ResReadMenuHeader( MenuHeader *currhead, WResFileID handle )
/***************************************************************/
{
    WResFileSSize   numread;

    numread = WRESREAD( handle, currhead, sizeof( MenuHeader ) );
    if( numread != sizeof( MenuHeader ) ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
        return( true );
    } else {
        return( false );
    }
}

bool ResReadMenuExtraBytes( MenuHeader *header, WResFileID handle, char *buf )
/***************************************************************************/
{
    bool            error;
    WResFileSSize   numread;
    int             size;

    error = false;
    numread = 0;
    size = header->HeaderSize * sizeof( uint_8 );
    if( buf != NULL ) {
        numread = WRESREAD( handle, buf, size );
        if( numread != size ) {
            error = true;
        }
    } else {
        WRESSEEK( handle, size, SEEK_CUR );
    }

    return( error );
}

// NB: Anyone using this function will have to manually seek back after
// calling ResIsMenuEx() (just as in ResIsDialogEx()).
// If you've already read the header, just call ResIsHeaderMenuEx().
bool ResIsMenuEx( WResFileID handle )
/***********************************/
{
    MenuHeader               header;
    bool                     ret;

    ret = ResReadMenuHeader( &header, handle );
    if( !ret ) {
        if( header.Version == MENUEX_VERSION_SIG ) {
            return( true );
        }
    }

    return( false );
}

bool ResIsHeaderMenuEx( MenuHeader *hdr )
/***************************************/
{
    return( hdr->Version == MENUEX_VERSION_SIG );
}

bool ResReadMenuExItem( MenuExItem *curritem, WResFileID handle )
/***************************************************************/
{
    bool               error;
    uint_32            type, state, id, helpId;
    uint_16            resInfo;

    state = 0;
    id = 0;
    resInfo = 0;

    // Store first structure members in temporary variables until
    // we know whether or not the item is a MenuExItemNormal or a
    // MenuExItemPopup

    error = ResReadUint32( &type, handle );
    if( !error ) {
        error = ResReadUint32( &state, handle );
    }
    if( !error ) {
        error = ResReadUint32( &id, handle );
    }
    if( !error ) {
        error = ResReadUint16( &resInfo, handle );
    }

    // Determine if this is a normal menu item or a popup menu item

    if( resInfo & MENUEX_POPUP ) {
        curritem->IsPopup = true;
        curritem->Item.ExPopup.Popup.ItemFlags = resInfo;
        curritem->Item.ExPopup.ExData.ItemId = id;
        curritem->Item.ExPopup.ExData.ItemType = type;
        curritem->Item.ExPopup.ExData.ItemState = state;
        curritem->Item.ExPopup.Popup.ItemText = ResRead32String( handle, NULL );

        // Careful! The string is DWORD aligned.
        ResPadDWord( handle );
        error = ResReadUint32( &helpId, handle );
        curritem->Item.ExPopup.ExData.HelpId = helpId;
    } else {
        curritem->IsPopup = false;
        curritem->Item.ExNormal.Normal.ItemFlags = resInfo;
        curritem->Item.ExNormal.Normal.ItemID = id;
        curritem->Item.ExNormal.Normal.ItemText = ResRead32String( handle, NULL );

        // Careful! The string is DWORD aligned.
        ResPadDWord( handle );
        curritem->Item.ExNormal.ExData.ItemType = type;
        curritem->Item.ExNormal.ExData.ItemState = state;
    }

    return( error );
}

bool ResReadMenuItem( MenuItem *curritem, WResFileID handle )
/***********************************************************/
{
    bool    error;
    uint_16 tmp16;

    error = ResReadUint16( &tmp16, handle );
    curritem->Item.Popup.ItemFlags = tmp16;
    if( !error ) {
        if( curritem->Item.Popup.ItemFlags & MENU_POPUP ) {
            curritem->IsPopup = true;
            curritem->Item.Popup.ItemText = ResReadString( handle, NULL );
            error = (curritem->Item.Popup.ItemText == NULL);
        } else {
            curritem->IsPopup = false;
            error = ResReadUint16( &tmp16, handle );
            curritem->Item.Normal.ItemID = tmp16;
            if( !error ) {
                curritem->Item.Normal.ItemText = ResReadString( handle, NULL );
                error = (curritem->Item.Normal.ItemText == NULL);
            }
        }
    }

    return( error );
}

bool ResReadMenuItem32( MenuItem *curritem, WResFileID handle )
/************************************************************/
{
    bool    error;
    uint_16 tmp16;

    error = ResReadUint16( &tmp16, handle );
    curritem->Item.Popup.ItemFlags = tmp16;
    if( !error ) {
        if( curritem->Item.Popup.ItemFlags & MENU_POPUP ) {
            curritem->IsPopup = true;
            curritem->Item.Popup.ItemText = ResRead32String( handle, NULL );
            error = (curritem->Item.Popup.ItemText == NULL);
        } else {
            curritem->IsPopup = false;
            error = ResReadUint16( &tmp16, handle );
            curritem->Item.Normal.ItemID = tmp16;
            if( !error ) {
                curritem->Item.Normal.ItemText = ResRead32String( handle, NULL );
                error = (curritem->Item.Normal.ItemText == NULL);
            }
        }
    }

    return( error );
}

MenuItem * ResNewMenuItem( void )
/*******************************/
{
    MenuItem *  newitem;

    newitem = WRESALLOC( sizeof( MenuItem ) );
    if( newitem == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        newitem->IsPopup = false;
        newitem->Item.Normal.ItemFlags = 0;
        newitem->Item.Normal.ItemID = 0;
        newitem->Item.Normal.ItemText = NULL;
    }

    return( newitem );
}

void ResFreeMenuItem( MenuItem * olditem )
/****************************************/
{
    if( olditem->IsPopup ) {
        if( olditem->Item.Popup.ItemText != NULL ) {
            WRESFREE( olditem->Item.Popup.ItemText );
        }
    } else {
        if( olditem->Item.Normal.ItemText != NULL ) {
            WRESFREE( olditem->Item.Normal.ItemText );
        }
    }

    WRESFREE( olditem );
}
