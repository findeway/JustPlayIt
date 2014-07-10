#include "stdafx.h"
#ifdef _WIN32

extern "C"
{
#include "sqlite3.h"
#include "sqliteInt.h"
#include "pager.h"
#include "btreeInt.h"
#include <wincrypt.h>



#define  SQLITE3_API_IMPLEMENT		//sqlite内部实现了的API,但是不在头文件声明
#define  SQLITE3_API_INTERFACE		//sqlite导出接口,外部需要实现
#define WX_PAGER_MJ_PGNO(x) ((PENDING_BYTE/(x))+1)

	typedef unsigned char u8;
	typedef unsigned int u32;
	typedef u32 Pgno;
	typedef long long int i64;
	typedef unsigned short u16;
	typedef short i16;

	struct Btree;
	struct PagerSavepoint;
	struct PCache;
	struct Bitvec;


	struct _Pager {
		sqlite3_vfs *pVfs;          /* OS functions to use for IO */
		u8 exclusiveMode;           /* Boolean. True if locking_mode==EXCLUSIVE */
		u8 journalMode;             /* On of the PAGER_JOURNALMODE_* values */
		u8 useJournal;              /* Use a rollback journal on this file */
		u8 noReadlock;              /* Do not bother to obtain readlocks */
		u8 noSync;                  /* Do not sync the journal if true */
		u8 fullSync;                /* Do extra syncs of the journal for robustness */
		u8 sync_flags;              /* One of SYNC_NORMAL or SYNC_FULL */
		u8 tempFile;                /* zFilename is a temporary file */
		u8 readOnly;                /* True for a read-only database */
		u8 memDb;                   /* True to inhibit all file I/O */

		/* The following block contains those class members that are dynamically
		** modified during normal operations. The other variables in this structure
		** are either constant throughout the lifetime of the pager, or else
		** used to store configuration parameters that affect the way the pager 
		** operates.
		**
		** The 'state' variable is described in more detail along with the
		** descriptions of the values it may take - PAGER_UNLOCK etc. Many of the
		** other variables in this block are described in the comment directly 
		** above this class definition.
		*/
		u8 state;                   /* PAGER_UNLOCK, _SHARED, _RESERVED, etc. */
		u8 dbModified;              /* True if there are any changes to the Db */
		u8 needSync;                /* True if an fsync() is needed on the journal */
		u8 journalStarted;          /* True if header of journal is synced */
		u8 changeCountDone;         /* Set after incrementing the change-counter */
		u8 setMaster;               /* True if a m-j name has been written to jrnl */
		u8 doNotSync;               /* Boolean. While true, do not spill the cache */
		u8 dbSizeValid;             /* Set when dbSize is correct */
		u8 subjInMemory;            /* True to use in-memory sub-journals */
		Pgno dbSize;                /* Number of pages in the database */
		Pgno dbOrigSize;            /* dbSize before the current transaction */
		Pgno dbFileSize;            /* Number of pages in the database file */
		int errCode;                /* One of several kinds of errors */
		int nRec;                   /* Pages journalled since last j-header written */
		u32 cksumInit;              /* Quasi-random value added to every checksum */
		u32 nSubRec;                /* Number of records written to sub-journal */
		Bitvec *pInJournal;         /* One bit for each page in the database file */
		sqlite3_file *fd;           /* File descriptor for database */
		sqlite3_file *jfd;          /* File descriptor for main journal */
		sqlite3_file *sjfd;         /* File descriptor for sub-journal */
		i64 journalOff;             /* Current write offset in the journal file */
		i64 journalHdr;             /* Byte offset to previous journal header */
		PagerSavepoint *aSavepoint; /* Array of active savepoints */
		int nSavepoint;             /* Number of elements in aSavepoint[] */
		char dbFileVers[16];        /* Changes whenever database file changes */
		u32 sectorSize;             /* Assumed sector size during rollback */

		u16 nExtra;                 /* Add this many bytes to each in-memory page */
		i16 nReserve;               /* Number of unused bytes at end of each page */
		u32 vfsFlags;               /* Flags for sqlite3_vfs.xOpen() */
		int pageSize;               /* Number of bytes in a page */
		Pgno mxPgno;                /* Maximum allowed size of the database */
		char *zFilename;            /* Name of the database file */
		char *zJournal;             /* Name of the journal file */
		int (*xBusyHandler)(void*); /* Function to call when busy */
		void *pBusyHandlerArg;      /* Context argument for xBusyHandler */
#ifdef SQLITE_TEST
		int nHit, nMiss;            /* Cache hits and missing */
		int nRead, nWrite;          /* Database pages read/written */
#endif
		void (*xReiniter)(DbPage*); /* Call this routine when reloading pages */
		void *(*xCodec)(void*,void*,unsigned int,int); /* Routine for en/decoding data */
		void (*xCodecSizeChng)(void*,int,int); /* Notify of page size changes */
		void (*xCodecFree)(void*);             /* Destructor for the codec */
		void *pCodec;               /* First argument to xCodec... methods */

		char *pTmpSpace;            /* Pager.pageSize bytes of space for tmp use */
		i64 journalSizeLimit;       /* Size limit for persistent journal files */
		PCache *pPCache;            /* Pointer to page cache object */
		sqlite3_backup *pBackup;    /* Pointer to list of ongoing backup processes */
	};

	SQLITE3_API_INTERFACE int sqlite3_key(sqlite3 *db, const void *pKey, int nKey);
	SQLITE3_API_INTERFACE int sqlite3_rekey(sqlite3 *db, const void *pKey, int nKey);
	SQLITE3_API_INTERFACE int sqlite3CodecAttach(sqlite3*, int, const void*, int);
	SQLITE3_API_INTERFACE void *sqlite3PagerGetCodec(_Pager *pPager);
	SQLITE3_API_INTERFACE void sqlite3PagerSetCodec(
		_Pager *pPager,
		void *(*xCodec)(void*,void*,Pgno,int),
		void (*xCodecSizeChng)(void*,int,int),
		void (*xCodecFree)(void*),
		void *pCodec);

	SQLITE3_API_INTERFACE void sqlite3CodecSizeChange(void *pArg, int pageSize, int reservedSize);
	SQLITE3_API_INTERFACE void sqlite3CodecFree(void*);
	SQLITE3_API_INTERFACE void sqlite3CodecGetKey(sqlite3 *db, int nDb, void **ppKey, int *pnKeyLen);
	SQLITE3_API_INTERFACE void sqlite3_activate_see(const char *info);

};

int sqlite3_key(sqlite3 *db, const void *pKey, int nKey)
{
	return sqlite3CodecAttach(db, 0, pKey, nKey);

}


namespace fxsys
{
	namespace fxsqlite3
	{

		HCRYPTPROV g_hProvider = 0; // Global instance of the cryptographic provider


		// Extra padding before and after the cryptographic buffer
#define CRYPT_OFFSET 8
		typedef struct _CRYPTBLOCK
		{
			_Pager    *pPager;       // Pager this cryptblock belongs to
			HCRYPTKEY hReadKey;     // Key used to read from the database and write to the journal
			HCRYPTKEY hWriteKey;    // Key used to write to the database
			DWORD     dwPageSize;   // Size of pages
			LPVOID    pvCrypt;      // A buffer for encrypting/decrypting (if necessary)
			DWORD     dwCryptSize;  // Equal to or greater than dwPageSize.  If larger, pvCrypt is valid and this is its size
		} CRYPTBLOCK, *LPCRYPTBLOCK;


		_Pager* _sqlite3BtreePager(Btree *p)
		{
			return (_Pager*)p->pBt->pPager;
		}
		// Create a cryptographic context.  Use the enhanced provider because it is available on
		// most platforms
		static BOOL InitializeProvider()
		{
			if (g_hProvider) return TRUE;

			if (!CryptAcquireContext(&g_hProvider, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
			{
				return FALSE;
			}
			return TRUE;
		}

		// Create or update a cryptographic context for a pager.
		// This function will automatically determine if the encryption algorithm requires
		// extra padding, and if it does, will create a temp buffer big enough to provide
		// space to hold it.
		static LPCRYPTBLOCK CreateCryptBlock(HCRYPTKEY hKey, _Pager *pager, LPCRYPTBLOCK pExisting)
		{
			LPCRYPTBLOCK pBlock;

			if (!pExisting) // Creating a new cryptblock
			{
				pBlock = (LPCRYPTBLOCK)sqlite3_malloc(sizeof(CRYPTBLOCK));
				ZeroMemory(pBlock, sizeof(CRYPTBLOCK));
				pBlock->hReadKey = hKey;
				pBlock->hWriteKey = hKey;
			}
			else // Updating an existing cryptblock
			{
				pBlock = pExisting;
			}

			pBlock->pPager = pager;
			pBlock->dwPageSize = (DWORD)pager->pageSize;
			pBlock->dwCryptSize = pBlock->dwPageSize;

			// Existing cryptblocks may have a buffer, if so, delete it
			if (pBlock->pvCrypt)
			{
				sqlite3_free(pBlock->pvCrypt);
				pBlock->pvCrypt = NULL;
			}

			// Figure out how big to make our spare crypt block
			CryptEncrypt(hKey, 0, TRUE, 0, NULL, &pBlock->dwCryptSize, pBlock->dwCryptSize * 2);
			pBlock->pvCrypt = sqlite3_malloc(pBlock->dwCryptSize + (CRYPT_OFFSET * 2));

			return pBlock;
		}

		// Destroy a cryptographic context and any buffers and keys allocated therein
		static void DestroyCryptBlock(void* pVBlock)
		{
			LPCRYPTBLOCK pBlock = (LPCRYPTBLOCK)pVBlock;
			// Destroy the read key if there is one
			if (pBlock->hReadKey)
			{
				CryptDestroyKey(pBlock->hReadKey);
			}

			// If there's a writekey and its not equal to the readkey, destroy it
			if (pBlock->hWriteKey && pBlock->hWriteKey != pBlock->hReadKey)
			{
				CryptDestroyKey(pBlock->hWriteKey);
			}

			// If there's extra buffer space allocated, free it as well
			if (pBlock->pvCrypt)
			{
				sqlite3_free(pBlock->pvCrypt);
			}

			// All done with this cryptblock
			sqlite3_free(pBlock);
		}

		// Needed for re-keying
		static void * sqlite3_pager_get_codecarg(_Pager *pPager)
		{
			return sqlite3PagerGetCodec(pPager);
		}

		// Derive an encryption key from a user-supplied buffer
		static HCRYPTKEY DeriveKey(const void *pKey, int nKeyLen)
		{
			HCRYPTHASH hHash = 0;
			HCRYPTKEY  hKey;

			if (!pKey || !nKeyLen) return 0;

			if (!InitializeProvider())
			{
				return MAXDWORD;
			}

			if (CryptCreateHash(g_hProvider, CALG_SHA1, 0, 0, &hHash))
			{
				if (CryptHashData(hHash, (LPBYTE)pKey, nKeyLen, 0))
				{
					CryptDeriveKey(g_hProvider, CALG_RC4, hHash, 0, &hKey);
				}
				CryptDestroyHash(hHash);
			}  
			return hKey;
		}

		// Encrypt/Decrypt functionality, called by pager.c
		void * sqlite3Codec(void *pArg, void *data, Pgno nPageNum, int nMode)
		{
			LPCRYPTBLOCK pBlock = (LPCRYPTBLOCK)pArg;
			DWORD dwPageSize;
			LPVOID pvTemp;

			if (!pBlock) return data;

			// Make sure the page size for the pager is still the same as the page size
			// for the cryptblock.  If the user changed it, we need to adjust!
			if (nMode != 2)
			{
				if (pBlock->pPager->pageSize != pBlock->dwPageSize)
				{
					// Update the cryptblock to reflect the new page size
					CreateCryptBlock(pBlock->hReadKey, pBlock->pPager, pBlock);
				}
			}

			switch(nMode)
			{
			case 0: // Undo a "case 7" journal file encryption
			case 2: // Reload a page
			case 3: // Load a page
				if (!pBlock->hReadKey) break;

				/* Block ciphers often need to write extra padding beyond the 
				data block.  We don't have that luxury for a given page of data so
				we must copy the page data to a buffer that IS large enough to hold
				the padding.  We then encrypt the block and write the buffer back to
				the page without the unnecessary padding.
				We only use the special block of memory if its absolutely necessary. */
				if (pBlock->dwCryptSize != pBlock->dwPageSize)
				{
					CopyMemory(((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET, data, pBlock->dwPageSize);
					pvTemp = data;
					data = ((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET;
				}

				dwPageSize = pBlock->dwCryptSize;
				CryptDecrypt(pBlock->hReadKey, 0, TRUE, 0, (LPBYTE)data, &dwPageSize);

				// If the encryption algorithm required extra padding and we were forced to encrypt or
				// decrypt a copy of the page data to a temp buffer, then write the contents of the temp
				// buffer back to the page data minus any padding applied.
				if (pBlock->dwCryptSize != pBlock->dwPageSize)
				{
					CopyMemory(pvTemp, data, pBlock->dwPageSize);
					data = pvTemp;
				}
				break;
			case 6: // Encrypt a page for the main database file
				if (!pBlock->hWriteKey) break;

				CopyMemory(((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET, data, pBlock->dwPageSize);
				data = ((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET;

				dwPageSize = pBlock->dwPageSize;
				CryptEncrypt(pBlock->hWriteKey, 0, TRUE, 0, ((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET, &dwPageSize, pBlock->dwCryptSize);
				break;
			case 7: // Encrypt a page for the journal file
				/* Under normal circumstances, the readkey is the same as the writekey.  However,
				when the database is being rekeyed, the readkey is not the same as the writekey.
				The rollback journal must be written using the original key for the
				database file because it is, by nature, a rollback journal.
				Therefore, for case 7, when the rollback is being written, always encrypt using
				the database's readkey, which is guaranteed to be the same key that was used to
				read the original data.
				*/
				if (!pBlock->hReadKey) break;

				CopyMemory(((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET, data, pBlock->dwPageSize);
				data = ((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET;

				dwPageSize = pBlock->dwPageSize;
				CryptEncrypt(pBlock->hReadKey, 0, TRUE, 0, ((LPBYTE)pBlock->pvCrypt) + CRYPT_OFFSET, &dwPageSize, pBlock->dwCryptSize);
				break;
			}

			return data;
		}

	}
}

using namespace fxsys::fxsqlite3;
#define SQLITECRYPTERROR_PROVIDER "Cryptographic provider not available"

// Called by sqlite and sqlite3_key_interop to attach a key to a database.
int sqlite3CodecAttach(sqlite3 *db, int nDb, const void *pKey, int nKeyLen)
{
	int rc = SQLITE_ERROR;
	HCRYPTKEY hKey = 0;

	// No key specified, could mean either use the main db's encryption or no encryption
	if (!pKey || !nKeyLen)
	{
		if (!nDb)
		{
			return SQLITE_OK; // Main database, no key specified so not encrypted
		}
		else // Attached database, use the main database's key
		{
			// Get the encryption block for the main database and attempt to duplicate the key
			// for use by the attached database
			LPCRYPTBLOCK pBlock = (LPCRYPTBLOCK)sqlite3_pager_get_codecarg(_sqlite3BtreePager(db->aDb[0].pBt));

			if (!pBlock) return SQLITE_OK; // Main database is not encrypted so neither will be any attached database
			if (!pBlock->hReadKey) return SQLITE_OK; // Not encrypted

			if (!CryptDuplicateKey(pBlock->hReadKey, NULL, 0, &hKey))
				return rc; // Unable to duplicate the key
		}
	}
	else // User-supplied passphrase, so create a cryptographic key out of it
	{
		hKey = DeriveKey(pKey, nKeyLen);
		if (hKey == MAXDWORD)
		{
			sqlite3Error(db, rc, SQLITECRYPTERROR_PROVIDER);
			return rc;
		}
	}

	// Create a new encryption block and assign the codec to the new attached database
	if (hKey)
	{
		LPCRYPTBLOCK pBlock = CreateCryptBlock(hKey, _sqlite3BtreePager(db->aDb[nDb].pBt), NULL);
		sqlite3PagerSetCodec((_Pager*)_sqlite3BtreePager(db->aDb[nDb].pBt), sqlite3Codec, sqlite3CodecSizeChange, sqlite3CodecFree,pBlock);
		rc = SQLITE_OK;
	}
	return rc;
}


void sqlite3CodecSizeChange(void *pArg, int pageSize, int reservedSize)
{

}

// 释放与一个页相关的加密块
void sqlite3CodecFree(void *pArg)
{
	if (pArg)
		DestroyCryptBlock((LPCRYPTBLOCK)pArg);
}

// Once a password has been supplied and a key created, we don't keep the 
// original password for security purposes.  Therefore return NULL.
void sqlite3CodecGetKey(sqlite3 *db, int nDb, void **ppKey, int *pnKeyLen)
{
	*ppKey = NULL;
	*pnKeyLen = 0;
}

void sqlite3_activate_see(const char *info)
{
}

int sqlite3_rekey( sqlite3 *db, const void *pKey, int nKeySize )
{
	Btree *pbt = db->aDb[0].pBt;
	Pager *p = sqlite3BtreePager(pbt);
	LPCRYPTBLOCK pBlock = (LPCRYPTBLOCK)sqlite3PagerGetCodec((_Pager*)p);
	HCRYPTKEY hKey = DeriveKey(pKey, nKeySize);
	int rc = SQLITE_ERROR;

	if (hKey == MAXDWORD)
	{
		sqlite3Error(db, rc, SQLITECRYPTERROR_PROVIDER);
		return rc;
	}

	if (!pBlock && !hKey) return SQLITE_OK; // Wasn't encrypted to begin with

	// To rekey a database, we change the writekey for the pager.  The readkey remains
	// the same
	if (!pBlock) // Encrypt an unencrypted database
	{
		pBlock = CreateCryptBlock(hKey, (_Pager*)p, NULL);
		pBlock->hReadKey = 0; // Original database is not encrypted
		sqlite3PagerSetCodec((_Pager*)sqlite3BtreePager(pbt), sqlite3Codec, sqlite3CodecSizeChange, sqlite3CodecFree,pBlock);
	}
	else // Change the writekey for an already-encrypted database
	{
		pBlock->hWriteKey = hKey;
	}

	// Start a transaction
	rc = sqlite3BtreeBeginTrans(pbt, 1);

	if (!rc)
	{
		// Rewrite all the pages in the database using the new encryption key
		int nPageCount = -1;
		int rc = sqlite3PagerPagecount(p, &nPageCount);
		Pgno nPage = (Pgno)nPageCount;
		int pageSize = sqlite3BtreeGetPageSize(pbt);
		Pgno nSkip = WX_PAGER_MJ_PGNO(pageSize);
		DbPage *pPage;
		Pgno n;

		for(n = 1; rc == SQLITE_OK && n <= nPage; n ++)
		{
			if (n == nSkip) continue;
			rc = sqlite3PagerGet(p, n, &pPage);
			if(!rc)
			{
				rc = sqlite3PagerWrite(pPage);
				sqlite3PagerUnref(pPage);
			}
		}
	}

	// If we succeeded, try and commit the transaction
	if (!rc)
	{
		rc = sqlite3BtreeCommit(pbt);
	}

	// If we failed, rollback
	if (rc)
	{
		sqlite3BtreeRollback(pbt);
	}

	// If we succeeded, destroy any previous read key this database used
	// and make the readkey equal to the writekey
	if (!rc)
	{
		if (pBlock->hReadKey)
		{
			CryptDestroyKey(pBlock->hReadKey);
		}
		pBlock->hReadKey = pBlock->hWriteKey;
	}
	// We failed.  Destroy the new writekey (if there was one) and revert it back to
	// the original readkey
	else
	{
		if (pBlock->hWriteKey)
		{
			CryptDestroyKey(pBlock->hWriteKey);
		}
		pBlock->hWriteKey = pBlock->hReadKey;
	}

	// If the readkey and writekey are both empty, there's no need for a codec on this
	// pager anymore.  Destroy the crypt block and remove the codec from the pager.
	if (!pBlock->hReadKey && !pBlock->hWriteKey)
	{
		sqlite3PagerSetCodec((_Pager*)p, NULL, NULL,NULL,NULL);
	}

	return rc;
}

SQLITE3_API_INTERFACE void * sqlite3PagerGetCodec( _Pager *pPager )
{
	return pPager->pCodec;
}

SQLITE3_API_INTERFACE void sqlite3PagerSetCodec( _Pager *pPager, void *(*xCodec)(void*,void*,Pgno,int), void (*xCodecSizeChng)(void*,int,int), void (*xCodecFree)(void*), void *pCodec )
{
	if( pPager->xCodecFree ) pPager->xCodecFree(pPager->pCodec);
	pPager->xCodec = pPager->memDb ? 0 : xCodec;
	pPager->xCodecSizeChng = xCodecSizeChng;
	pPager->xCodecFree = xCodecFree;
	pPager->pCodec = pCodec;

	if( pPager->xCodecSizeChng )
	{
		pPager->xCodecSizeChng(pPager->pCodec, pPager->pageSize,
			(int)pPager->nReserve);
	}
}

#endif