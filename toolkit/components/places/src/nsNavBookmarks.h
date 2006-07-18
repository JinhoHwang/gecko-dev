/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Places.
 *
 * The Initial Developer of the Original Code is
 * Google Inc.
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Brian Ryner <bryner@brianryner.com> (original author)
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef nsNavBookmarks_h_
#define nsNavBookmarks_h_

#include "nsINavBookmarksService.h"
#include "nsIStringBundle.h"
#include "nsITransaction.h"
#include "nsNavHistory.h"
#include "nsNavHistoryResult.h" // need for Int64 hashtable
#include "nsBrowserCompsCID.h"

class nsNavBookmarks : public nsINavBookmarksService,
                       public nsINavHistoryObserver
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSINAVBOOKMARKSSERVICE
  NS_DECL_NSINAVHISTORYOBSERVER

  nsNavBookmarks();
  nsresult Init();

  // called by nsNavHistory::Init
  static nsresult InitTables(mozIStorageConnection* aDBConn);

  static nsNavBookmarks* GetBookmarksService() {
    if (!sInstance) {
      nsresult rv;
      nsCOMPtr<nsINavBookmarksService> serv(do_GetService(NS_NAVBOOKMARKSSERVICE_CONTRACTID, &rv));
      NS_ENSURE_SUCCESS(rv, nsnull);
      NS_ASSERTION(sInstance, "Should have static instance pointer now");
    }
    return sInstance;
  }

  nsresult AddBookmarkToHash(PRInt64 aBookmarkId, PRTime aMinTime);

  nsresult ResultNodeForFolder(PRInt64 aID, nsNavHistoryQueryOptions *aOptions,
                               nsNavHistoryResultNode **aNode);

  // Find all the children of a folder, using the given query and options.
  // For each child, a ResultNode is created and added to |children|.
  // The results are ordered by folder position.
  nsresult QueryFolderChildren(PRInt64 aFolderId,
                               nsNavHistoryQueryOptions *aOptions,
                               nsCOMArray<nsNavHistoryResultNode> *children);

  // If aFolder is -1, use the autoincrement id for folder index. 
  nsresult CreateFolderWithID(PRInt64 aFolder, PRInt64 aParent, 
                              const nsAString& title, PRInt32 aIndex,
                              PRInt64* aNewFolder);
  nsresult CreateContainerWithID(PRInt64 aFolder, PRInt64 aParent, 
                                 const nsAString& title, PRInt32 aIndex, 
                                 const nsAString& type, PRInt64* aNewFolder);

  // Returns a statement to get information about a folder id
  mozIStorageStatement* DBGetFolderInfo() { return mDBGetFolderInfo; }
  // constants for the above statement
  static const PRInt32 kGetFolderInfoIndex_FolderID;
  static const PRInt32 kGetFolderInfoIndex_Title;
  static const PRInt32 kGetFolderInfoIndex_Type;

private:
  static nsNavBookmarks *sInstance;

  ~nsNavBookmarks();

  nsresult InitRoots();
  nsresult CreateRoot(mozIStorageStatement* aGetRootStatement,
                      const nsCString& name, PRInt64* aID,
                      PRBool* aWasCreated);

  nsresult AdjustIndices(PRInt64 aFolder,
                         PRInt32 aStartIndex, PRInt32 aEndIndex,
                         PRInt32 aDelta);
  PRInt32 FolderCount(PRInt64 aFolder);
  nsresult GetFolderType(PRInt64 aFolder, nsACString &aType);

  // remove me when there is better query initialization
  nsNavHistory* History() { return nsNavHistory::GetHistoryService(); }

  mozIStorageStatement* DBGetURLPageInfo()
  { return History()->DBGetURLPageInfo(); }

  mozIStorageConnection* DBConn() { return History()->GetStorageConnection(); }

  nsMaybeWeakPtrArray<nsINavBookmarkObserver> mObservers;
  PRInt64 mRoot;
  PRInt64 mBookmarksRoot;
  PRInt64 mToolbarRoot;
  PRInt64 mTagRoot;

  // the level of nesting of batches, 0 when no batches are open
  PRInt32 mBatchLevel;

  // true if the outermost batch has an associated transaction that should
  // be committed when our batch level reaches 0 again.
  PRBool mBatchHasTransaction;

  // This stores a mapping from all pages reachable by redirects from bookmarked
  // pages to the bookmarked page. Used by GetBookmarkedURIFor.
  nsDataHashtable<nsTrimInt64HashKey, PRInt64> mBookmarksHash;
  nsresult FillBookmarksHash();
  nsresult RecursiveAddBookmarkHash(PRInt64 aBookmarkId, PRInt64 aCurrentSource,
                                    PRTime aMinTime);
  nsresult UpdateBookmarkHashOnRemove(PRInt64 aBookmarkId);

  nsresult GetParentAndIndexOfFolder(PRInt64 aFolder, PRInt64* aParent, 
                                     PRInt32* aIndex);

  nsresult IsBookmarkedInDatabase(PRInt64 aBookmarkID, PRBool* aIsBookmarked);

  nsCOMPtr<mozIStorageStatement> mDBGetFolderInfo;    // kGetFolderInfoIndex_* results

  nsCOMPtr<mozIStorageStatement> mDBGetChildren;       // kGetInfoIndex_* results + kGetChildrenIndex_* results
  static const PRInt32 kGetChildrenIndex_Position;
  static const PRInt32 kGetChildrenIndex_ItemChild;
  static const PRInt32 kGetChildrenIndex_FolderChild;
  static const PRInt32 kGetChildrenIndex_FolderTitle;

  nsCOMPtr<mozIStorageStatement> mDBFindURIBookmarks;  // kFindBookmarksIndex_* results
  static const PRInt32 kFindBookmarksIndex_ItemChild;
  static const PRInt32 kFindBookmarksIndex_FolderChild;
  static const PRInt32 kFindBookmarksIndex_Parent;
  static const PRInt32 kFindBookmarksIndex_Position;

  nsCOMPtr<mozIStorageStatement> mDBFolderCount;

  nsCOMPtr<mozIStorageStatement> mDBIndexOfItem;
  nsCOMPtr<mozIStorageStatement> mDBIndexOfFolder;
  nsCOMPtr<mozIStorageStatement> mDBGetChildAt;

  nsCOMPtr<mozIStorageStatement> mDBGetRedirectDestinations;

  // keywords
  nsCOMPtr<mozIStorageStatement> mDBGetKeywordForURI;
  nsCOMPtr<mozIStorageStatement> mDBGetURIForKeyword;

  nsCOMPtr<nsIStringBundle> mBundle;

  class RemoveFolderTransaction : public nsITransaction {
  public:
    RemoveFolderTransaction(PRInt64 aID, PRInt64 aParent, 
                            const nsAString& aTitle, PRInt32 aIndex,
                            const nsACString& aType) 
                            : mID(aID),
                              mParent(aParent),
                              mIndex(aIndex){
      mTitle = aTitle;
      mType = aType;
    }
    
    NS_DECL_ISUPPORTS

    NS_IMETHOD DoTransaction() {
      nsNavBookmarks* bookmarks = nsNavBookmarks::GetBookmarksService();
      return bookmarks->RemoveFolder(mID);
    }

    NS_IMETHOD UndoTransaction() {
      nsNavBookmarks* bookmarks = nsNavBookmarks::GetBookmarksService();
      PRInt64 newFolder;
      if (mType.IsEmpty())
        return bookmarks->CreateFolderWithID(mID, mParent, mTitle, mIndex, &newFolder);
      nsAutoString type; type.AssignWithConversion(mType);
      return bookmarks->CreateContainerWithID(mID, mParent, mTitle, mIndex, type, &newFolder); 
    }

    NS_IMETHOD RedoTransaction() {
      return DoTransaction();
    }

    NS_IMETHOD GetIsTransient(PRBool* aResult) {
      *aResult = PR_FALSE;
      return NS_OK;
    }
    
    NS_IMETHOD Merge(nsITransaction* aTransaction, PRBool* aResult) {
      *aResult = PR_FALSE;
      return NS_OK;
    }

  private:
    PRInt64 mID;
    PRInt64 mParent;
    nsString mTitle;
    nsCString mType;
    PRInt32 mIndex;
  };

  // in nsBookmarksHTML
  nsresult ImportBookmarksHTMLInternal(nsIURI* aURL,
                                       PRBool aAllowRootChanges,
                                       PRInt64 aFolder);
};

struct nsBookmarksUpdateBatcher
{
  nsBookmarksUpdateBatcher() { nsNavBookmarks::GetBookmarksService()->BeginUpdateBatch(); }
  ~nsBookmarksUpdateBatcher() { nsNavBookmarks::GetBookmarksService()->EndUpdateBatch(); }
};


#endif // nsNavBookmarks_h_
