/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*  Xuikon CXnResource-class defines all information what is needed to maintain
*  resources of Application Theme. Every CXnResource-objects defines just one 
*  resource source, however, the same resource source could be referenced
*  multiple times inside a theme - resources are usually shared. 
*  Xuikon Application Theme Management system supports theme resourcing by 
*  offering resource conversations and storing services, and offering Rendering
*  Engines the following services:
*  1) the way to match a resource quoted in xml- or css-module to the 
*  corresponding resource source, and 2) a secure way to access a resource 
*  through the Symbian Platform Security's process separation wall.
*
*
*/


#ifndef XNRESOURCE_H
#define XNRESOURCE_H

#include <e32base.h>
#include <apmstd.h> // MIME
#include <s32mem.h> // streams
 
/**
* TXnLockingPolicy
* Locking policy flag-definitions for Xuikon. These are bit-masked.
*/
enum TXnLockingPolicy
  {
  EXnUnlocked           = 0x0000, //0b0000000000000000,
  EXnLocked             = 0x0001  //0b0000000000000001,
  };
    
/**
* TXnCacheType
* Cache-type definitions for Xuikon.
*/
enum TXnCacheType
    {
    /**
    * ECacheNone. Resource is not cached = direct file access in resources original format.
    *
    */
    ECacheNone,
    
    /**
    * ECacheFile. Resource is externalized to file stream - it can be internalized from given 
    *               file stream on construct. 
    */
    ECacheFile, 
    
    /**
    * ECacheNone. Resource is externalized in to iBuffer - it can be internalized from this
    *               buffer on construct.
    */  
    ECacheMemory
    };

/**
*   Resource file extension definitions for Xuikon.
*/
_LIT(KRLBFileExtension,".r");
_LIT(KODTFileExtension,".o");
_LIT(KDTDFileExtension,".dtd");
_LIT(KXMLFileExtension,".xml");
_LIT(KCSSFileExtension,".css");
_LIT(KDATFileExtension,".dat");

_LIT(KFBSFileExtension,".fbs");
_LIT(KMBMFileExtension,".mbm");
_LIT(KMIFFileExtension,".mif");
_LIT(KBMPFileExtension,".bmp");
_LIT(KJPGFileExtension,".jpg");
_LIT(KJPEGFileExtension,".jpeg");
_LIT(KPNGFileExtension,".png");
_LIT(KGIFFileExtension,".gif");
_LIT(KAACFileExtension,".aac");
_LIT(KWAVFileExtension,".wav");
_LIT(KMIDFileExtension,".mid");
_LIT(KMP3FileExtension,".mp3");
_LIT(KM3GFileExtension,".m3g");
_LIT(KSVGFileExtension,".svg");
_LIT(KSWFFileExtension,".swf");
_LIT(KTXTFileExtension,".txt");

/**
* TXnResourceType
* Resource-type definitions for Xuikon.
*/
enum TXnResourceType
    {
    /**
    * EResourceNone. No type at all - juts a file path.
    */
    EResourceNone,
    
    /**
    * EResourceRLB.
    */
    EResourceRLB,
    
    /**
    * EResourceODT. Resource is ODT (Xuikon Application Theme's binary format)
    */
    EResourceODT,
    
    /**
    * EResourceDTD.
    */
    EResourceDTD,
    
    /**
    * EResourceXML.
    */
    EResourceXML,
    
    /**
    * EResourceCSS.
    */
    EResourceCSS,
    
    /**
    * EResourceDAT.
    */
    EResourceDAT,
    
    /**
    * EResourceRES. Resource is some media content.
    */
    EResourceRES,
    
    /**
    * EResourceFBS. Resource is externalized CFbsBitmap.
    */
    EResourceFBS,
    
    /**
    * EResourceMBM. Resource is Symbian MBM-multiple-image.
    */
    EResourceMBM,
    
    /**
    * EResourceMIF. Resource is Symbian MIF.
    */
    
    EResourceMIF,
    
    /**
    * EResourceBMP. Resource is BMP-image.
    */
    EResourceBMP,
    
    /**
    * EResourceJPG and EResourceJPEG. Resource is JPEG-compliant image.
    */
    EResourceJPG,
    EResourceJPEG,
    
    /**
    * EResourcePNG. Resource is in PNG-format.
    */
    EResourcePNG,
    
    /**
    * EResourceGIF. Resourec is in GIF-format. 
    */
    EResourceGIF,  
    
    /**
    * EResourceAAC. Resource is a AAC audio file.
    */
    EResourceAAC, 
    
    /**
    * EResourceWAV. Resource is a WAV audio file
    */
    EResourceWAV,
    
    /**
    * EResourceMID. Resource is a MID audio file.
    */
    EResourceMID,
    
    /**
    * EResourceMP3. Resource is a MP3 audio file.
    */
    EResourceMP3,
        
    /**
    * EResourceM3G. Resource is M3G.
    */
    EResourceM3G,
    
    /**
    * EResourceSVG. Resource is SVG. 
    */
    EResourceSVG,
       
    /**
    * EResourceSWF. Resource is a Macromedia shock wave i.e. flash file
    */
    EResourceSWF,
    
    /**
    * EResourceMIME. Resource is in Symbian OS 9.x supported MIME-type format.
    */
    EResourceMIME,

    /**
    * EResourceTXT. Resource is TXT.
    */
    EResourceTXT
    
    };


/**
*  @ingroup group_xnresource
*  
*  CXnResource is Xuikon-utility class defining resource-structure.
*
*  @lib XnResource.lib
*  @since S60 3.1
*/
class CXnResource : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXnResource* NewL();
        /**
        * Two-phased constructor.
        * @param aStream The read stream
        */
        IMPORT_C static CXnResource* NewLC( RReadStream& aStream );
        
        /**
        * CXnResource::CloneL(). Makes a clone of this class.
        * @since S60 3.1
        * @return void
        */
        IMPORT_C CXnResource* CloneL();
        /**
        * Destructor.
        */
        virtual ~CXnResource();

    public: // New functions
        
        /**
        * CXnResource::SetLockingPolicy(). Sets value of iLockingPolicy data member.
        * @since S60 3.1
        * @param aLockingPolicy is new value for iLockingPolicy data member.
        * @return void
        */
        IMPORT_C void SetLockingPolicy( TXnLockingPolicy aLockingPolicy );
        
        /**
        * CXnResource::LockingPolicy(). Gets value of iLockingPolicy data member.
        * @since S60 3.1
        * @return TXnLockingPolicy iLockingPolicy.
        */
        IMPORT_C TXnLockingPolicy LockingPolicy() const;
        
        /**
        * CXnResource::SetCacheType(). Sets value of iCacheType data member.
        * @since S60 3.1
        * @param aCacheType is new value for iCacheType data member.
        * @return void
        */
        IMPORT_C void SetCacheType( TXnCacheType aCacheType );
        
        /**
        * CXnResource::CacheType(). Gets value of iCacheType data member.
        * @since S60 3.1
        * @return TXnCacheType iCacheType.
        */
        IMPORT_C TXnCacheType CacheType() const;
     
        /**
        * CXnResource::SetResourceType(). Sets value of iResourceType data member.
        * @since S60 3.1
        * @param aResourceType is new value of iResourceType data member.
        * @return void
        */    
        IMPORT_C void SetResourceType( TXnResourceType aResourceType );
        
        /**
        * CXnResource::ResourceType(). Gets value of iResourceType data member..
        * @since S60 3.1
        * @return TXnResourceType iResourceType.
        */
        IMPORT_C TXnResourceType ResourceType() const;
        
        /**
        * CXnResource::SetResourceIdL(). Sets value of iResourceId data member.
        * @since S60 3.1
        * @param aResourceId is new value of iResourceId data member.
        * @return void
        */
        
        IMPORT_C void SetResourceIdL( const TDesC& aResourceId );
        
        /**
        * CXnResource::ResourceId(). Gets value of iResourceId data member.
        * @since S60 3.1
        * @return TDesC& iResourceId.
        */
        IMPORT_C const TDesC& ResourceId() const;
        
        /**
        * CXnResource::SetNameSpaceL(). Sets value of iNameSpace data member.
        * @since S60 3.1
        * @param aNameSpace is new value of iNameSpace data member.
        * @return void
        */
        IMPORT_C void SetNameSpaceL( const TDesC& aNameSpace );
        
        /**
        * CXnResource::NameSpace(). Gets value of iNameSpace data member.
        * @since S60 3.1
        * @return TDesC& iNameSpace
        */
        IMPORT_C const TDesC& NameSpace() const;
        
        /**
        * CXnResource::SetFileNameL(). Sets value of iFileName data member.
        * @since S60 3.1
        * @param aFileName is new iFileName value.
        * @return void
        */
        IMPORT_C void SetFileNameL( const TDesC& aFileName );
        
        /**
        * CXnResource::FileName(). Gets value of iFileName data member.
        * @since S60 3.1
        * @return TDes& iFileName.
        */
        IMPORT_C const TDesC& FileName() const;
        
        /**
        * CXnResource::SetMimeTypeL(). Sets value of iMimeType data member. 
        * @since S60 3.1
        * @param aDataType is a new value of iMimeType.
        * @return void
        */
        IMPORT_C void SetMimeTypeL( const TDataType aDataType );
        
        /**
        * CXnResource::MimeType(). Get iMimeType of this CXnResource-object.
        * @since S60 3.1
        * @return TDataType iMimeType.
        */
        IMPORT_C TDataType MimeType() const;
        
        /**
        * CXnResource::WriteStream(). Gets write stream on iBuffer.
        * @since S60 3.1
        * @param aOffset a buffer position of iBuffer to write data.
        * @return RBufWriteStream& write stream.
        */
        IMPORT_C RBufWriteStream& WriteStream( TUint aOffset = 0 );      
        
        /**
        * CXnResource::ReadStream(). Gets read stream on iBuffer.
        * @since S60 3.1
        * @param aOffset a buffer position of iBuffer to read data. 
        * @return RBufReadStream& read stream
        */
        IMPORT_C RBufReadStream& ReadStream(TUint aOffset = 0 );

        /**
        * CXnResource::SetOffset(). Sets internal data pointer of iBuffer.
        * @since S60 3.1
        * @param aOffset is new offset of iBuffer data pointer.
        * @return void
        */
        IMPORT_C void SetOffset( TUint aOffset );
        
        /**
        * CXnResource::Offset(). Internal iBuffer data pointer position.
        * @since S60 3.1
        * @return TUint iOffset pointing iBuffer data pointer.
        */
        IMPORT_C TUint Offset() const;
        
        /**
        * CXnResource::ExternalizeL(). Externalizes CXnResource-object's member data.
        * @since S60 3.1
        * @param aStream is a write stream given.
        * @return void
        */
        IMPORT_C void ExternalizeL( RWriteStream& aStream) const;
        
        /**
        * CXnResource::InternalizeL(). Internalizes CXnResource-object's member data.
        * @since S60 3.1
        * @param aStream is data read stream given.
        * @return void
        */
        IMPORT_C void InternalizeL( RReadStream& aStream);
        
        /**
        * CXnResource::GetDataStreamLength().
        * @since S60 3.1
        * @return TInt data length of CXnResource-data members for streaming functions.
        */
        IMPORT_C TInt GetDataStreamLength() const;
        
    private:

        /**
        * C++ default constructor.
        */
        CXnResource();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:     // Data
        TXnLockingPolicy iLockingPolicy;
        TXnCacheType iCacheType;
        TXnResourceType iResourceType;
        TDesC* iResourceID;
        TDesC* iNameSpace;
        TDesC* iFileName;
        TDataType iMimeType;
        CBufSeg* iBuffer;
        TUint iOffset;
        RBufWriteStream iWriteStream;
        RBufReadStream iReadStream;

         // Reserved pointer for future extension
        TAny* iReserved;
    };

#endif      // XNRESOURCE_H   
// End of File
