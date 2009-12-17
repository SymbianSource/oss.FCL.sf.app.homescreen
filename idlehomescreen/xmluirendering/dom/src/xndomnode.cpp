/*
 * Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Primary datatype for the entire Document Object Model.
 *
 */

// INCLUDE FILES
#include    "xndomnode.h"
#include    "xndomattribute.h"
#include    "xndomproperty.h"
#include    "xndomlist.h"
#include    "xndomstringpool.h"

// LOCAL CONSTANTS AND MACROS
const TInt KNotDefined = -1;
_LIT8( KIdAttr, "id" );
_LIT8( KRefAttr, "ref" );
const TInt KPropertyListGranularity = 8;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXnDomNode::CXnDomNode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXnDomNode::CXnDomNode(CXnDomStringPool& aStringPool) :
    iStringPool(aStringPool), iNodeId(KNotDefined)
    {
    }

// -----------------------------------------------------------------------------
// CXnDomNode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXnDomNode::ConstructL(const TDesC8& aName, const TDesC8& aNS)
    {
    iNameRef = iStringPool.AddStringL(aName);
    iNSRef = iStringPool.AddStringL(aNS);

    iChildList = CXnDomList::NewL(CXnDomList::ENodeList, iStringPool);
    iAttributeList
            = CXnDomList::NewL(CXnDomList::EAttributeList, iStringPool);
    iPropertyList = CXnDomList::NewL(CXnDomList::EPropertyList, iStringPool,
            KPropertyListGranularity);

    }

// -----------------------------------------------------------------------------
// CXnDomNode::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CXnDomNode* CXnDomNode::NewL(const TDesC8& aName, const TDesC8& aNS,
        CXnDomStringPool& aStringPool)
    {
    CXnDomNode* self = new (ELeave) CXnDomNode(aStringPool);

    CleanupStack::PushL(self);
    self->ConstructL(aName, aNS);
    CleanupStack::Pop(self);

    return self;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
CXnDomNode* CXnDomNode::NewL(RReadStream& aStream,
        CXnDomStringPool& aStringPool)
    {
    CXnDomNode* self = new (ELeave) CXnDomNode(aStringPool);

    CleanupStack::PushL(self);

    aStream >> *self;
    CleanupStack::Pop(self);

    return self;
    }

// Destructor
CXnDomNode::~CXnDomNode()
    {
    delete iChildList;
    delete iAttributeList;
    delete iPropertyList;

    delete iPCData;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::CloneL
// Clones this node and it's child nodes. This is a recursive function.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomNode::CloneL(CXnDomStringPool& aStringPool)
    {
    const TDesC8& name = iStringPool.String(iNameRef);
    const TDesC8& ns = iStringPool.String(iNSRef);

    CXnDomNode* clone = CXnDomNode::NewL(name, ns, aStringPool);
    CleanupStack::PushL(clone);
    if (iPCData)
        {
        clone->AppendPCDataL(*iPCData);
        }
    clone->iNodeId = iNodeId;
    clone->iRefNode = iRefNode;

    TInt childCount(iChildList->Length());

    for (TInt i = 0; i < childCount; i++)
        {
        CXnDomNode* childClone =
                static_cast<CXnDomNode*> (iChildList->Item(i))->CloneL(
                        aStringPool);
        CleanupStack::PushL(childClone);
        childClone->iParentNode = clone;
        clone->iChildList->AddItemL(childClone);
        CleanupStack::Pop(childClone);
        }

    TInt attrCount(iAttributeList->Length());
    for (TInt j = 0; j < attrCount; j++)
        {
        CXnDomAttribute
                * attrClone =
                        static_cast<CXnDomAttribute*> (iAttributeList->Item(j))->CloneL(
                                aStringPool);
        CleanupStack::PushL(attrClone);
        clone->iAttributeList->AddItemL(attrClone);
        CleanupStack::Pop(attrClone);
        }

    TInt propertyCount(iPropertyList->Length());
    for (TInt k = 0; k < propertyCount; k++)
        {
        CXnDomProperty
                * propClone =
                        static_cast<CXnDomProperty*> (iPropertyList->Item(k))->CloneL(
                                aStringPool);
        CleanupStack::PushL(propClone);
        clone->iPropertyList->AddItemL(propClone);
        CleanupStack::Pop(propClone);
        }
    CleanupStack::Pop(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::CloneWithoutKidsL
// Clones only this node. This is a recursive function.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomNode::CloneWithoutKidsL(
        CXnDomStringPool& aStringPool)
    {
    const TDesC8& name = iStringPool.String(iNameRef);
    const TDesC8& ns = iStringPool.String(iNSRef);

    CXnDomNode* clone = CXnDomNode::NewL(name, ns, aStringPool);
    CleanupStack::PushL(clone);
    if (iPCData)
        {
        clone->AppendPCDataL(*iPCData);
        }
    clone->iNodeId = iNodeId;
    clone->iRefNode = iRefNode;

    TInt attrCount(iAttributeList->Length());
    for (TInt j = 0; j < attrCount; j++)
        {
        CXnDomAttribute
                * attrClone =
                        static_cast<CXnDomAttribute*> (iAttributeList->Item(j))->CloneL(
                                aStringPool);
        CleanupStack::PushL(attrClone);
        clone->iAttributeList->AddItemL(attrClone);
        CleanupStack::Pop(attrClone);
        }

    TInt propertyCount(iPropertyList->Length());
    for (TInt k = 0; k < propertyCount; k++)
        {
        CXnDomProperty
                * propClone =
                        static_cast<CXnDomProperty*> (iPropertyList->Item(k))->CloneL(
                                aStringPool);
        CleanupStack::PushL(propClone);
        clone->iPropertyList->AddItemL(propClone);
        CleanupStack::Pop(propClone);
        }
    CleanupStack::Pop(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::CreateRefNodeL
// Recursive function to create referer nodes.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomNode::CreateRefNodeL()
    {
    const TDesC8& name = iStringPool.String(iNameRef);
    const TDesC8& ns = iStringPool.String(iNSRef);

    CXnDomNode* ref = CXnDomNode::NewL(name, ns, iStringPool);
    CleanupStack::PushL(ref);

    ref->iRefNode = ETrue;

    TInt childCount(iChildList->Length());

    for (TInt i = 0; i < childCount; i++)
        {
        CXnDomNode
                * childRef =
                        static_cast<CXnDomNode*> (iChildList->Item(i))->CreateRefNodeL();
        CleanupStack::PushL(childRef);
        childRef->iParentNode = ref;
        ref->iChildList->AddItemL(childRef);
        CleanupStack::Pop(childRef);
        }

    CXnDomAttribute* attr = NULL;
    if (!iRefNode)
        {
        attr = static_cast<CXnDomAttribute*> (iAttributeList->FindByName(
                KIdAttr));
        }
    else
        {
        attr = static_cast<CXnDomAttribute*> (iAttributeList->FindByName(
                KRefAttr));
        }

    if (attr)
        {
        CXnDomAttribute* newAttr = CXnDomAttribute::NewL(KRefAttr,
                iStringPool);
        CleanupStack::PushL(newAttr);
        newAttr->SetValueL(attr->Value());
        ref->iAttributeList->AddItemL(newAttr);
        CleanupStack::Pop(newAttr);
        }
    else
        {
        //referred node don't have an id or ref, thats not ok.
        User::Leave(KErrArgument);
        }

    CleanupStack::Pop(ref);
    return ref;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::Name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnDomNode::Name()
    {
    return iStringPool.String(iNameRef);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::Namespace
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CXnDomNode::Namespace()
    {
    return iStringPool.String(iNSRef);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::AttributeList
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomList& CXnDomNode::AttributeList() const
    {
    return *iAttributeList;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::PropertyList
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomList& CXnDomNode::PropertyList()
    {
    return *iPropertyList;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::SetParent
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomNode::SetParent(CXnDomNode* aParent)
    {
    iParentNode = aParent;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::Parent
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomNode::Parent() const
    {
    return iParentNode;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::AddChildL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomNode::AddChildL(CXnDomNode* aNode)
    {
    aNode->SetParent(this);
    iChildList->AddItemL(aNode);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::AddChildL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomNode::AddChildL(CXnDomNode* aNode, TInt aIndex)
    {
    aNode->SetParent(this);
    iChildList->AddItemL(aNode, aIndex);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::DeleteChild
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomNode::DeleteChild(CXnDomNode* aNode)
    {
    iChildList->DeleteItem(aNode);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::ReplaceChildL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomNode::ReplaceChildL(CXnDomNode* aNode,
        CXnDomNode* aNewNode)
    {
    CXnDomNode* swapChild = NULL;
    CXnDomList& childList = aNode->ChildNodes();
    TInt childCount(childList.Length());
    for (TInt i = 0; i < childCount; i++)
        {
        swapChild = static_cast<CXnDomNode*> (childList.Item(i));
        aNewNode->AddChildL(swapChild); //Let the new node adopt the child
        childList.RemoveItem(swapChild); //Remove it from the orginal parent
        }
    iChildList->DeleteItem(aNode); // Delete the old child

    aNewNode->SetParent(this); //Set new child
    iChildList->AddItemL(aNewNode);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::ChildNodes
// -----------------------------------------------------------------------------
//   
EXPORT_C CXnDomList& CXnDomNode::ChildNodes()
    {
    return *iChildList;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::SetNodeId
// -----------------------------------------------------------------------------
//   
EXPORT_C void CXnDomNode::SetNodeId(const TInt aNodeId)
    {
    iNodeId = aNodeId;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::NodeId
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt CXnDomNode::NodeId() const
    {
    return iNodeId;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::ItemIndex
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt CXnDomNode::ItemIndex(const MXnDomListItem& aItem) const
    {
    return iChildList->ItemIndex(aItem);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::AppendPCDataL
// -----------------------------------------------------------------------------
//   
EXPORT_C void CXnDomNode::AppendPCDataL(const TDesC8& aPCData)
    {
    if (iPCData)
        {
        iPCData = iPCData->ReAllocL(iPCData->Length() + aPCData.Length());
        iPCData->Des().Append(aPCData);
        }
    else
        {
        iPCData = aPCData.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// CXnDomNode::PCData
// -----------------------------------------------------------------------------
//  
EXPORT_C const TDesC8& CXnDomNode::PCData()
    {
    if (iPCData)
        {
        return *iPCData;
        }
    return KNullDesC8;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::SetPCDataL
// -----------------------------------------------------------------------------
//  
EXPORT_C void CXnDomNode::SetPCDataL(const TDesC8& aPCData)
    {
    if (iPCData)
        {
        delete iPCData;
        iPCData = NULL;
        }
    iPCData = aPCData.AllocL();
    }

// -----------------------------------------------------------------------------
// CXnDomNode::ContentType
// -----------------------------------------------------------------------------
//   
EXPORT_C const TContentType& CXnDomNode::ContentType()
    {
    return iContentType;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::SetContentType
// -----------------------------------------------------------------------------
//   
EXPORT_C void CXnDomNode::SetContentType(const TContentType& aContentType)
    {
    iContentType = aContentType;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::StringPool
// -----------------------------------------------------------------------------
//        
EXPORT_C CXnDomStringPool& CXnDomNode::StringPool() const
    {
    return iStringPool;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::SetRefNode
// -----------------------------------------------------------------------------
//        
EXPORT_C void CXnDomNode::SetRefNode(TBool aRefNode)
    {
    iRefNode = aRefNode;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::IsRefNode
// -----------------------------------------------------------------------------
//        
EXPORT_C TBool CXnDomNode::IsRefNode() const
    {
    return iRefNode;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::Size
// -----------------------------------------------------------------------------
//
TInt CXnDomNode::Size() const
    {
    TInt size(0);

    size += sizeof(TInt16); //iNameRef
    size += sizeof(TInt16); //iNSRef
    size += sizeof(TBool); //iRefNode
    if (iPCData)
        {
        size += sizeof(TInt8);
        size += sizeof(TInt16);
        size += iPCData->Size();
        size++;
        }
    else
        {
        size += sizeof(TInt8);
        }

    size += sizeof(TInt32); //For nodeId
    size += iAttributeList->Size();
    size += iPropertyList->Size();

    size += iChildList->Size();

    return size;
    }
// -----------------------------------------------------------------------------
// CXnDomNode::ExternalizeL
// NOTE: This function is not implemented yet!
// -----------------------------------------------------------------------------
//
void CXnDomNode::ExternalizeL(RWriteStream& aStream) const
    {

    aStream.WriteInt16L(iNameRef);
    aStream.WriteInt16L(iNSRef);
    aStream.WriteInt8L(iRefNode);

    if (iPCData)
        {
        aStream.WriteInt8L(ETrue);
        aStream.WriteInt16L(iPCData->Length());
        aStream << *iPCData;
        }
    else
        {
        aStream.WriteInt8L(EFalse);
        }

    aStream.WriteInt32L(iNodeId);
    aStream << *iChildList;
    aStream << *iAttributeList;
    aStream << *iPropertyList;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::InternalizeL
// -----------------------------------------------------------------------------
//
void CXnDomNode::InternalizeL(RReadStream& aStream)
    {
    iNameRef = aStream.ReadInt16L() + iStringPool.Offset();
    iNSRef = aStream.ReadInt16L() + iStringPool.Offset();
    iRefNode = aStream.ReadInt8L();

    TInt len(0);
    TBool exist(aStream.ReadInt8L());
    if (exist)
        {
        len = aStream.ReadInt16L();
        delete iPCData;
        iPCData = NULL;
        iPCData = HBufC8::NewL(aStream, len);
        }

    iNodeId = aStream.ReadInt32L();

    iChildList = CXnDomList::NewL(aStream, iStringPool);
    TInt count(iChildList->Length());
    for (TInt i = 0; i < count; i++)
        {
        CXnDomNode* node = static_cast<CXnDomNode*> (iChildList->Item(i));
        if (node)
            {
            node->SetParent(this);
            }
        else
            {
            User::Leave(KErrArgument);
            }
        }

    iAttributeList = CXnDomList::NewL(aStream, iStringPool);

    iPropertyList = CXnDomList::NewL(aStream, iStringPool);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::ReadL
// -----------------------------------------------------------------------------
//
void CXnDomNode::ReadL(RReadStream& aStream)
    {
    iNameRef = aStream.ReadInt16L();
    iNSRef = aStream.ReadInt16L();
    iRefNode = aStream.ReadInt8L();

    TBool exist(aStream.ReadInt8L());
    if (exist)
        {
        TInt len(0);
        len = aStream.ReadInt16L();
        delete iPCData;
        iPCData = NULL;
        iPCData = HBufC8::NewL(aStream, len);
        }

    iNodeId = aStream.ReadInt32L();

    iChildList = CXnDomList::NewL(aStream, iStringPool);
    TInt count(iChildList->Length());
    for (TInt i = 0; i < count; i++)
        {
        CXnDomNode* node = static_cast<CXnDomNode*> (iChildList->Item(i));
        if (node)
            {
            node->SetParent(this);
            }
        else
            {
            User::Leave(KErrArgument);
            }
        }

    iAttributeList = CXnDomList::NewL(aStream, iStringPool);

    iPropertyList = CXnDomList::NewL(aStream, iStringPool);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::DescendantCount
// Recursive counting function
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt CXnDomNode::DescendantCount() const
    {
    TInt count(1); //Node itself

    TInt length(iChildList->Length());
    for (TInt i = 0; i < length; i++)
        {
        count
                += static_cast<CXnDomNode*> (iChildList->Item(i))->DescendantCount();
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::DeleteAttributeList
// Deletes the attribute list
// -----------------------------------------------------------------------------
//   
EXPORT_C void CXnDomNode::DeleteAttributeList()
    {
    delete iAttributeList;
    iAttributeList = NULL;

    }

// -----------------------------------------------------------------------------
// CXnDomNode::AttributeValue
// Returns value of "name" attribute
// -----------------------------------------------------------------------------
//   
EXPORT_C const TDesC8& CXnDomNode::AttributeValue(const TDesC8& aAttribute) const
    {
    CXnDomAttribute* attribute =
            static_cast<CXnDomAttribute*> (iAttributeList->FindByName(
                    aAttribute));
    if (attribute)
        {
        return attribute->Value();
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CXnDomNode::SetLayoutNode
// Sets pointer to associated layout node
// -----------------------------------------------------------------------------
//   
EXPORT_C void CXnDomNode::SetLayoutNode(CXnNode* aNode)
    {
    iLayoutNode = aNode;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::LayoutNode
// Gets pointer to associated layout node
// -----------------------------------------------------------------------------
//   
EXPORT_C CXnNode* CXnDomNode::LayoutNode()
    {
    return iLayoutNode;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::SetNamespaceL
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomNode::SetNamespaceL(const TDesC8& aNS)
    {
    iNSRef = iStringPool.AddStringL(aNS);
    }

// -----------------------------------------------------------------------------
// CXnDomNode::CloneL
// Clones this node and it's child nodes and sets new namespace. This is a
// recursive function.
// -----------------------------------------------------------------------------
//
EXPORT_C CXnDomNode* CXnDomNode::CloneL(CXnDomStringPool& aStringPool,
        const TDesC8& aNS)
    {
    const TDesC8& name = iStringPool.String(iNameRef);

    CXnDomNode* clone = CXnDomNode::NewL(name, aNS, aStringPool);
    CleanupStack::PushL(clone);
    if (iPCData)
        {
        clone->AppendPCDataL(*iPCData);
        }
    clone->iNodeId = iNodeId;
    clone->iRefNode = iRefNode;

    TInt childCount(iChildList->Length());

    for (TInt i = 0; i < childCount; i++)
        {
        CXnDomNode* childClone =
                static_cast<CXnDomNode*> (iChildList->Item(i))->CloneL(
                        aStringPool, aNS);
        CleanupStack::PushL(childClone);
        childClone->iParentNode = clone;
        clone->iChildList->AddItemL(childClone);
        CleanupStack::Pop(childClone);
        }

    TInt attrCount(iAttributeList->Length());
    for (TInt j = 0; j < attrCount; j++)
        {
        CXnDomAttribute
                * attrClone =
                        static_cast<CXnDomAttribute*> (iAttributeList->Item(j))->CloneL(
                                aStringPool);
        CleanupStack::PushL(attrClone);
        clone->iAttributeList->AddItemL(attrClone);
        CleanupStack::Pop(attrClone);
        }

    TInt propertyCount(iPropertyList->Length());
    for (TInt k = 0; k < propertyCount; k++)
        {
        CXnDomProperty
                * propClone =
                        static_cast<CXnDomProperty*> (iPropertyList->Item(k))->CloneL(
                                aStringPool);
        CleanupStack::PushL(propClone);
        clone->iPropertyList->AddItemL(propClone);
        CleanupStack::Pop(propClone);
        }
    CleanupStack::Pop(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// CXnDomNode::SetOwnershipL
// Clones this node and it's child nodes and sets new namespace. This is a
// recursive function.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXnDomNode::SetOwnershipL(const TDesC8& aNS)
    {
    TInt childCount(iChildList->Length());

    SetNamespaceL(aNS);
    for (TInt i = 0; i < childCount; i++)
        {
        CXnDomNode* child = static_cast<CXnDomNode*> (iChildList->Item(i));
        child->SetOwnershipL(aNS);
        }
    }

//  End of File  
