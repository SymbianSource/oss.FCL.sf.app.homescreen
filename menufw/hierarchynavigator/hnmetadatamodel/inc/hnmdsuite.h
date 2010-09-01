/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*/


#ifndef C_HNMDSUITE_H
#define C_HNMDSUITE_H

#include <e32base.h>
#include <badesca.h>

#include "hnmdmodel.h"
#include "hnsuitemodel.h"
#include "hnglobals.h"
#include "hnmdmodel.h"
#include "hnqueryresultcollector.h"

class CHnSuiteModelContainer;
class CHnFilter;
class CHnMdItem;
class CHnMdLocalization;
class CHnItemId;
class CHnMdQueries;
class CHnMdEventMapping;

/**
 * Suite.
 * 
 * This is the representation of the suite. The crucial member variable
 * of this suite is the list of the items.
 *
 * @lib hnmetadatamodel
 * @since S60 5.0
 * @ingroup group_hnmetadatamodel
 */
NONSHARABLE_CLASS(CHnMdSuite) : public CBase,
    public MHnQueryResultCollectorNotification
    {
public:

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdSuite* NewL( TXmlEngElement aElement, THnMdCommonPointers* aCmnPtrs );

    /**
     * Two-phase constructor.
     *
     * @since S60 5.0
     * @param aElement Element.
     * @param aCmnPtrs Common pointers.
     * @return Fully constructed object.
     */
    static CHnMdSuite* NewLC( TXmlEngElement aElement, THnMdCommonPointers* aCmnPtrs );

    /**
     * Standard C++ virtual destructor.
     */
    virtual ~CHnMdSuite();

    /**
     * Evaluates suite.
     *
     * @since S60 5.0
     * @param aSuiteModel Suite model.
     * @return Status code. 
     */
    TInt EvaluateL( CHnSuiteModel & aSuiteModel );

    /**
     * Marks according to filter which item should be evaluated.
     *
     * @since S60 5.0
     * @param aFilter Filter.
     * @param aSuiteModel Suite Model to get Custom Id
     */
    void MarkEvaluationL( CHnFilter& aFilter, CHnSuiteModel& aSuiteModel );
    
    /**
     * Handles back event.
     *
     * @since S60 ?S60_version
     * @param aMulContainer Suite model container.
     * @param aGenre Suite's name.
     * @return Error code.
     */
    TInt HandleBackEventL( CHnSuiteModelContainer* aMulContainer, TDesC* aGenre );
    
    /**
     * Switches between edit mode and normal mode
     *
     * @since S60 5.0
     * @param aMode Mode, e.g. normal / edit.
     */
    void SetModeL( TMdMode aMode );
    
    /**
     * Returns mode type.
     * 
     * @return Mode type, e.g. normal / edit. 
     */
    TMdMode Mode();
    
    /**
     * Returns name of the suite.
     * 
     * @since S60 v3.2
     * @return Name of the suite.
     */          
    IMPORT_C const TDesC& SuiteName();
    
    /**
     * Return suite parameters.
     * 
     * @return Suite parameters.
     */
    IMPORT_C CLiwGenericParamList& GetSuiteParameters();
    
    /**
     * Sets suite parameters.
     * 
     * @param aParams Suite parameters.
     */
    IMPORT_C void SetSuiteParametersL(CLiwGenericParamList& aParams);
    
    
    /**
     * Returns alternative widget number.
     * 
     * @since S60 v5.0
     * @return Alternative widget number.
     */ 
    IMPORT_C TInt GetAlternativeWidgetNumber();

private:

    /**
     * Set genre.
     *
     * @since S60 5.0
     * @param aGenre Genre descriptor.
     */
    void SetGenreL(const TDesC& aGenre);

    /**
     * Set genre.
     *
     * @since S60 5.0
     * @param aAttr Xml attribute.
     */
    void SetGenreL(TXmlEngAttr aAttr);
    
    /**
     * Set template.
     *
     * @since S60 5.0
     * @param aTemplate Template descriptor.
     */
    void SetTemplateL( const TDesC8& aTemplate );
    
    /**
     * Set template.
     *
     * @since S60 5.0
     * @param aTemplate Xml attribute.
     */
    void SetTemplateL( TXmlEngAttr aTemplate );
    
    /**
     * Sets widget type.
     *
     * @since S60 5.0
     * @param aWidgetType Xml attribute.
     */
    void SetWidgetTypeL( TXmlEngAttr aWidgetType );
    
    /**
     * Sets allowable widget types.
     *
     * @since S60 5.0
     * @param aWidgetType Contains widget types.
     */
    void SetWidgetTypeL( TDesC8& aWidgetType );

    /**
     * Sets allowable widget types and store it in CR.
     *
     * @since S60 5.0
     * @param aWidgetType Widget type.
     */
    void SaveWidgetTypeL( TDesC8& aWidgetType );
    
    /**
     * Sets title.
     *
     * @since S60 5.0
     * @param aTitle Xml attribute.
     */
    void SetTitleL( TXmlEngAttr aTitle );
    
    /**
     * Sets empty text.
     *
     * @since S60 5.0
     * @param aEmptyText The text displayed for empty suite.
     */
    void SetEmptyTextL( TXmlEngAttr aEmptyText );

    /**
     * Evaluate suite itself.
     *
     * @since S60 5.0
     * @param aSuiteModel Suite model.
     */
    void EvaluateSuiteItselfL( CHnSuiteModel & aSuiteModel );

    /**
     * Creates items.
     *
     * @since S60 5.0
     * @param aEntries Xml element.
     * @param aCmnPtrs Common pointers.
     */
    void CreateItemsL(TXmlEngElement aEntries,
            THnMdCommonPointers* aCmnPtrs );

    /**
     * Gets direct child property.
     *
     * @since S60 5.0
     * @param iIndex Index.
     * @param aPropertyType Property type.
     * @param aResult Result variant.
     * @return Status.
     */
    TBool GetDirectChildProperty( TInt iIndex,
            TPropertyType aPropertyType,
            TLiwVariant& aResult );
    
    /**
     * Called when results have finished being collected.
     * 
     * @param aResults The results.
     */
    void ResultsCollectedL( CLiwGenericParamList* aResults );
    
    /**
     * Sets custom id using parameters.
     * 
     * @param aSuiteModel Suite model.
     * @param aParams Parameters used to set custom id.
     * @param aPos Positions of the custom id in aParams.
     */
    void SetCustomIdL( CHnSuiteModel* aSuiteModel, 
    		const CLiwGenericParamList& aParams, TInt aPos );
    
    /**
     * Sets exit mode for a suite.
     */
    void SetExitModeL();


private:

    /**
     * Standar C++ constructor.
     * 
     * @since S60 5.0
     * @param aCmnPtrs Common pointers.
     */
    CHnMdSuite( THnMdCommonPointers* aCmnPtrs );

    /**
     * Restores widget type for this suite from central repository.
     *
     * @return Error code.
     */
    TInt ReadWidgetTypeL();
    
    /**
     * Saves widget type for this suite in central repository.
     *
     * @return Error code.
     */
    TInt SaveWidgetTypeL() const;
        
    /**
     * Checks if the widget type is stored in service 
     * and saves it in given descriptor.
     *
     * @param aParams Contains data from service.
     * @param aWidgetType Widget type to be set.
     * @return True if widget was set, False 
     *      when no widget stored in service.
     */
    TBool ReadWidgetTypeFromServiceL( CLiwGenericParamList& aParams, 
            RBuf8& aWidgetType );
    
    /**
     * Sets the appropriate widget flag for allowable widgets.
     * 
     * @param aConfDes Configuration string.
     */ 
    void DoSetWidgetTypeL( TDesC8 & aConfDes );
    
    /**
     * Sets the alternative template for widget switch.
     * 
     * @param aOption Position of widget in xml configuration.
     */ 
    void SetAlternativeTemplateL( TInt aOption );
    
    /**
     * Sets the alternative widget.
     * 
     * @param aParams List of data from service.
     */ 
    void SetAlternativeWidgetL( CLiwGenericParamList& aParams );
    
    /**
     * Sets the alternative template.
     * 
     * @param aParams List of data from service.
     */ 
    void SetAlternativeTemplateL( CLiwGenericParamList& aParams );
        
    /**
     * Standard symbian 2nd phase constructor.
     * 
     * @since S60 5.0
     * @param element Xml element.
     */
    void ConstructL( TXmlEngElement element );
    
    /**
     * Adds widget type to generic param list passed to
     * items when evaluating the suite.
     * 
     * The information about widget type can be retrieved
     * using the following path:
     *      mm:/suite:type
     * 
     * @param aParamList a param list into which the wigdet
     *                   type is added.
     * 
     */
    void AddInternalStateL( CLiwGenericParamList& aParamList );
    

    /**
     * Sets custom identifier.
     * 
     * @since S60 5.50
     * @param aElement Xml element.
     */
    void SetCustomIdL( TXmlEngElement aElement );
    
    /**
     * Sets highlight according to parameters passed in uri.
     */
    void SetUriBasedHighlightL();
    


private: // data

    /**
     * Own - Suite name.
     */
    RBuf iSuiteName;

    /**
     * Own - Title
     */
    RBuf8 iTitle;

    /**
     * Own - Empty text
     */
    RBuf8 iEmptyText;

    /**
     * Own - Items.
     */
    RPointerArray< CHnMdItem > iItems;

    /**
     * Common pointers.
     */
    THnMdCommonPointers* iCmnPtrs;
    
    /**
     * Template.
     */
    RBuf8 iTemplate;
    
    /**
     * Allowable templates.
     */
    CDesC8Array * iAllowableTemplate;
    
    /**
     * Widget type.
     */
    THnSuiteWidgetType iWidgetType;
     
    /**
     * Widget type.
     */
    RArray< THnSuiteWidgetType > iAllowableWidgetType;
    
    /**
     * Allowed types - as string.
     */
    RBuf8 iAllowedTypes;
    
    /**
     * Event mapping
     */
    CHnMdEventMapping* iEventMapping;
    
    /**
     * Queries
     */
    CHnMdQueries* iQueries;
    
    /**
     * Cached evaluation parameters, i.e. params:folder_id, etc.
     */
    CLiwGenericParamList* iSuiteParams;
    
    /**
     * Stores information about mode.
     */
    TMdMode iMode;
    
    /**
     * Own.
     */
    CHnQueryResultCollector* iQc;
    
    /**
     * Suite model.
     * Not Own
     */
    CHnSuiteModel* iSuiteModel;
    
    /**
     * Custom identifier. 
     */
    RBuf8 iCustomId8;

public:
    /**
     * Marks if evaulation should take place. 
     */    
    TBool iEvaluationNeeded;
    
    };

#endif // C_HNMMSUITE_H
