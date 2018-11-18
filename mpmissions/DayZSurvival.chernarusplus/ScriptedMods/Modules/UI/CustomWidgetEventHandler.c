class CustomWidgetEventHandler : UIScriptedMenu
{
	// Widget m_rootHudWidget;
	// TextWidget m_NotificationLabel;
	
	void CustomWidgetEventHandler()
	{
		// m_rootHudWidget = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_hud.layout");
		// m_NotificationLabel	= TextWidget.Cast( m_rootHudWidget.FindAnyWidget( "NoticiationLabellTextWidget" ) );
	}
	
    void ShowOutOfZone()
    {
		// GetGame().ChatPlayer(0, "Leaving Zone from Event Handler");
        // GetGame().GetUIManager().ShowDialog("OUT OF ZONE", "You are about to die!", 13374, DBT_OK, DBB_NONE, DMT_WARNING, this);
		
	
		// m_NotificationLabel.SetPos(0.5, 0.5);
		// m_NotificationLabel.SetText("Leaving Zone Widget");
		// m_NotificationLabel.Show(true);
	}
};