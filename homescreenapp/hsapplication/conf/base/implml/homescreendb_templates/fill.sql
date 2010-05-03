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