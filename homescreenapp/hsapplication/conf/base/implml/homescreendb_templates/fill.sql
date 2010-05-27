{%- set portraitWallpaper = feat_tree.HomeScreenDbSettings.portraitWallpaper._value or '' -%}
{%- set landscapeWallpaper = feat_tree.HomeScreenDbSettings.landscapeWallpaper._value or '' -%}
{%- set defaultPageId = feat_tree.HomeScreenDbSettings.defaultPageId._value -%}
{%- set maximumPageCount = feat_tree.HomeScreenDbSettings.maximumPageCount._value -%}
{%- set maximumWidgetHeight = feat_tree.HomeScreenDbSettings.maximumWidgetHeight._value -%}
{%- set maximumWidgetWidth = feat_tree.HomeScreenDbSettings.maximumWidgetWidth._value -%}
{%- set minimumWidgetHeight = feat_tree.HomeScreenDbSettings.minimumWidgetHeight._value -%}
{%- set minimumWidgetWidth = feat_tree.HomeScreenDbSettings.minimumWidgetWidth._value -%}
INSERT INTO Scene (portraitWallpaper, landscapeWallpaper, defaultPageId, maximumPageCount, maximumWidgetHeight, maximumWidgetWidth, minimumWidgetHeight, minimumWidgetWidth) VALUES ('{{portraitWallpaper}}', '{{landscapeWallpaper}}', {{defaultPageId}}, {{maximumPageCount}}, {{maximumWidgetHeight}}, {{maximumWidgetWidth}}, {{minimumWidgetHeight}}, {{minimumWidgetWidth}});

{% for index in range(feat_tree.HomeScreenDbSettings.Pages._value|length) -%}
    {%- set id = feat_tree.HomeScreenDbSettings.Pages.id._value[index] -%}
    {%- set indexPosition = feat_tree.HomeScreenDbSettings.Pages.indexPosition._value[index] -%}
    INSERT INTO Pages (id, indexPosition) VALUES ({{id}}, {{indexPosition}});
{% endfor %}

{% for index in range(feat_tree.HomeScreenDbSettings.Widgets._value|length) -%}
    {%- set id = feat_tree.HomeScreenDbSettings.Widgets.id._value[index] -%}
    {%- set uri = feat_tree.HomeScreenDbSettings.Widgets.uri._value[index] or '' -%}
    {%- set pageId = feat_tree.HomeScreenDbSettings.Widgets.pageId._value[index] -%}
    
    INSERT INTO Widgets (id, uri, pageId) VALUES ({{id}}, '{{uri}}', {{pageId}});
{% endfor %}

{% for index in range(feat_tree.HomeScreenDbSettings.WidgetPresentations._value|length) -%}
    {%- set key = feat_tree.HomeScreenDbSettings.WidgetPresentations.key._value[index] or '' -%}
    {%- set x = feat_tree.HomeScreenDbSettings.WidgetPresentations.x._value[index] -%}
    {%- set y = feat_tree.HomeScreenDbSettings.WidgetPresentations.y._value[index] -%}
    {%- set zValue = feat_tree.HomeScreenDbSettings.WidgetPresentations.zValue._value[index] -%}
    {%- set widgetId = feat_tree.HomeScreenDbSettings.WidgetPresentations.widgetId._value[index] -%}
    
    INSERT INTO WidgetPresentations (key, x, y, zValue, widgetId) VALUES ('{{key}}', {{x}}, {{y}}, {{zValue}}, {{widgetId}});
{% endfor %}

{% for index in range(feat_tree.HomeScreenDbSettings.WidgetPreferences._value|length) -%}
    {%- set key = feat_tree.HomeScreenDbSettings.WidgetPreferences.key._value[index] or '' -%}
    {%- set value = feat_tree.HomeScreenDbSettings.WidgetPreferences.value._value[index] -%}
    {%- set widgetId = feat_tree.HomeScreenDbSettings.WidgetPreferences.widgetId._value[index] -%}
    
    INSERT INTO WidgetPreferences (key, value, widgetId) VALUES ('{{key}}', '{{value}}', {{widgetId}});
{% endfor %}

{%- set snappingEnabled = feat_tree.HomeScreenDbSettings.SnapConfiguration.snappingEnabled._value -%}    
{%- set showEffects = feat_tree.HomeScreenDbSettings.SnapConfiguration.showEffects._value -%} 
{%- set snapForce = feat_tree.HomeScreenDbSettings.SnapConfiguration.snapForce._value -%}    
{%- set snapGap = feat_tree.HomeScreenDbSettings.SnapConfiguration.snapGap._value -%}    
{%- set borderGap = feat_tree.HomeScreenDbSettings.SnapConfiguration.borderGap._value -%}    
{%- set timeout = feat_tree.HomeScreenDbSettings.SnapConfiguration.timeout._value -%}    
INSERT INTO SnapConfiguration (snappingEnabled, showEffects, snapForce, snapGap, borderGap, timeout) VALUES ({{snappingEnabled}}, {{showEffects}}, {{snapForce}}, {{snapGap}}, {{borderGap}}, {{timeout}});

{%- set bounceEffect = feat_tree.HomeScreenDbSettings.GeneralConfiguration.bounceEffect._value -%}
{%- set tapAndHoldDistance = feat_tree.HomeScreenDbSettings.GeneralConfiguration.tapAndHoldDistance._value -%}
{%- set widgetTapAndHoldTimeout = feat_tree.HomeScreenDbSettings.GeneralConfiguration.widgetTapAndHoldTimeout._value -%}
{%- set sceneTapAndHoldTimeout = feat_tree.HomeScreenDbSettings.GeneralConfiguration.sceneTapAndHoldTimeout._value -%}
{%- set pageChangeZoneWidth = feat_tree.HomeScreenDbSettings.GeneralConfiguration.pageChangeZoneWidth._value -%}
{%- set pageIndicatorSpacing = feat_tree.HomeScreenDbSettings.GeneralConfiguration.pageIndicatorSpacing._value -%}
{%- set pageChangeAnimationDuration = feat_tree.HomeScreenDbSettings.GeneralConfiguration.pageChangeAnimationDuration._value -%}
{%- set pageChangeZoneAnimationDuration = feat_tree.HomeScreenDbSettings.GeneralConfiguration.pageChangeZoneAnimationDuration._value -%}
{%- set pageChangeZoneReverseAnimationDuration = feat_tree.HomeScreenDbSettings.GeneralConfiguration.pageChangeZoneReverseAnimationDuration._value -%}
{%- set pageRemovedAnimationDuration = feat_tree.HomeScreenDbSettings.GeneralConfiguration.pageRemovedAnimationDuration._value -%}
{%- set newPageAddedAnimationDuration = feat_tree.HomeScreenDbSettings.GeneralConfiguration.newPageAddedAnimationDuration._value -%}
{%- set widgetDragEffectDuration = feat_tree.HomeScreenDbSettings.GeneralConfiguration.widgetDragEffectDuration._value -%}
{%- set widgetDropEffectDuration = feat_tree.HomeScreenDbSettings.GeneralConfiguration.widgetDropEffectDuration._value -%}
{%- set boundaryFeedbackEffectDistance = feat_tree.HomeScreenDbSettings.GeneralConfiguration.boundaryFeedbackEffectDistance._value -%}
INSERT INTO GeneralConfiguration (bounceEffect, tapAndHoldDistance, widgetTapAndHoldTimeout, sceneTapAndHoldTimeout, pageChangeZoneWidth, pageIndicatorSpacing, pageChangeAnimationDuration, pageChangeZoneAnimationDuration, pageChangeZoneReverseAnimationDuration, pageRemovedAnimationDuration, newPageAddedAnimationDuration, widgetDragEffectDuration, widgetDropEffectDuration, boundaryFeedbackEffectDistance ) VALUES ({{bounceEffect}}, {{tapAndHoldDistance}}, {{widgetTapAndHoldTimeout}}, {{widgetTapAndHoldTimeout}}, {{pageChangeZoneWidth}}, {{pageIndicatorSpacing}}, {{pageChangeAnimationDuration}}, {{pageChangeZoneAnimationDuration}}, {{pageChangeZoneReverseAnimationDuration}}, {{pageRemovedAnimationDuration}}, {{newPageAddedAnimationDuration}}, {{widgetDragEffectDuration}}, {{widgetDropEffectDuration}}, {{boundaryFeedbackEffectDistance}});

{%- set pageChangeFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.pageChangeFeedbackType._value -%}
{%- set widgetPickFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.widgetPickFeedbackType._value -%}
{%- set widgetDropFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.widgetDropFeedbackType._value -%}
{%- set widgetRepositionFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.widgetRepositionFeedbackType._value -%}
{%- set widgetOverTrashbinFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.widgetOverTrashbinFeedbackType._value -%}
{%- set widgetDropToTrashbinFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.widgetDropToTrashbinFeedbackType._value -%}
{%- set shortcutWidgetTapFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.shortcutWidgetTapFeedbackType._value -%}
{%- set widgetMoveBlockedFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.widgetMoveBlockedFeedbackType._value -%}
{%- set clockWidgetTapFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.clockWidgetTapFeedbackType._value -%}
{%- set widgetSnappingFeedbackType = feat_tree.HomeScreenDbSettings.FeedbackConfiguration.widgetSnappingFeedbackType._value -%}
INSERT INTO FeedbackConfiguration (pageChangeFeedbackType, widgetPickFeedbackType, widgetDropFeedbackType, widgetRepositionFeedbackType, widgetOverTrashbinFeedbackType, widgetDropToTrashbinFeedbackType, shortcutWidgetTapFeedbackType, widgetMoveBlockedFeedbackType, clockWidgetTapFeedbackType, widgetSnappingFeedbackType) VALUES ('{{pageChangeFeedbackType}}', '{{widgetPickFeedbackType}}', '{{widgetDropFeedbackType}}', '{{widgetRepositionFeedbackType}}', '{{widgetOverTrashbinFeedbackType}}', '{{widgetDropToTrashbinFeedbackType}}', '{{shortcutWidgetTapFeedbackType}}', '{{widgetMoveBlockedFeedbackType}}', '{{clockWidgetTapFeedbackType}}', '{{widgetSnappingFeedbackType}}');

