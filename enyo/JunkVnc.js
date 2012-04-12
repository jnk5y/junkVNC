enyo.kind({
	name: "MyApps.junkVNC",
	kind: enyo.VFlexBox,
	components: [
		{kind: "ApplicationEvents", onWindowRotated: "resizePlugin"},
		{kind: "AppMenu", components: [
			{kind: "EditMenu", autoDisableItems: false},
			{caption: "About", onclick: "showAboutContent"}
		]},
		{kind: "Popup", name: "generalPopup", allowHtml: true},
		{kind: "PopupSelect", name: "popupKeys", onSelect: "keySelect", modal: false, dismissWithClick: false, dismissWithEscape: false, items: [
			"del","home","end","pgup","pgdn","esc","up","down","left","right"
		]},
		{kind: "ModalDialog", name: "newServerDialog", caption: "Add New Server", components: [
			{kind: "Group", components: [
				{kind: "Scroller", height: "350px", components: [
					{kind: "RowGroup", className: "enyo-first", caption: "Name", components: [
						{kind: enyo.Input, name: "newServerTitle"}
					]},
					{kind: "RowGroup", caption: "VNC Server Hostname/IP", components: [
						{kind: enyo.Input, name: "newServerURL", autoCapitalize: "lowercase"}
					]},
					{kind: "RowGroup", caption: "VNC Server Port", components: [
						{kind: enyo.Input, name: "newServerPort", value: 5900, autoCapitalize: "lowercase"}
					]},
					{kind: "RowGroup", className: "enyo-last", caption: "VNC Server Password", components: [
						{kind: enyo.Input, name: "newServerPassword", autoCapitalize: "lowercase"}
					]}
				]}
			]},
			{kind: enyo.HFlexBox, components: [
				{flex: 1},
				{kind: "Button", onclick: "addServer", caption: "Save", className: "enyo-button-affirmative"},
				{kind: "Button", onclick: "cancelAdd", caption: "Cancel"},
				{flex: 1}
			]}
		]},
		{kind: "ModalDialog", name: "editServerDialog", caption: "Edit Server", components: [
			{kind: "Group", components: [
				{kind: "Scroller", height: "350px", components: [
					{kind: "RowGroup", className: "enyo-first", caption: "Name", components: [
						{kind: enyo.Input, name: "editServerTitle"}
					]},
					{kind: "RowGroup", caption: "VNC Server Hostname/IP", components: [
						{kind: enyo.Input, name: "editServerURL", autoCapitalize: "lowercase"}
					]},
					{kind: "RowGroup", caption: "VNC Server Port", components: [
						{kind: enyo.Input, name: "editServerPort", value: 5900, autoCapitalize: "lowercase"}
					]},
					{kind: "RowGroup", className: "enyo-last", caption: "VNC Server Password", components: [
						{kind: enyo.Input, name: "editServerPassword", autoCapitalize: "lowercase"}
					]}
				]}
			]},
			{kind: enyo.HFlexBox, components: [
				{flex: 1},
				{kind: "Button", onclick: "editServer", caption: "Save", className: "enyo-button-affirmative"},
				{kind: "Button", onclick: "cancelEdit", caption: "Cancel"},
				{flex: 1}
			]}
		]},
		{kind: "SlidingPane", flex: 1, onSelect: "paneSelected", dragAnywhere: false, components: [
			{name: "ServerListPane", width: "250px", dragAnywhere: false, components: [ 
				{kind: "Header", content: "Servers"},
				{kind: enyo.Scroller, flex: 1, components: [
					{kind: enyo.VirtualList, name: "serverList", onSetupRow: "getServer", components: [
						{kind: enyo.SwipeableItem, name: "listItem", onConfirm: "deleteServerItem", onclick: "serverClick", layoutKind: enyo.HFlexLayout, tapHighlight: true, components: [
							{name: "listItemTitle", content: "", flex: 1}
						]}
					]}
				]},
				{kind: enyo.Toolbar, pack: "justify", components: [
					{kind: "Button", name: "connectButton", caption: "Connect", disabled: "true", onclick: "connectServer", className: "enyo-button-affirmative"},
					{flex: 1},
					{icon: "images/edit.png", name: "editButton", disabled: "true", onclick: "showEditServerPopup"},
					{icon: "images/new.png", onclick: "showAddNewServerPopup"}
				]}
			]},
			{name: "VncViewPane", flex: 1, dragAnywhere: false, onResize: "resizePlugin", components: [
				{kind: enyo.Hybrid, name: "plugin", height: 1, width: 1, executable: "junkvnc_plugin", takeKeyboardFocus: false, passTouchEvents: true, onPluginReady: "handlePluginReady", onPluginDisconnected: "pluginFail" },
				{flex: 1},
				{kind: enyo.Toolbar, name: "VncViewPaneBar", components: [
					{kind: enyo.GrabButton},
					{flex: 1},
					{kind: "Button", name: "disconnectButton", caption: "Disconnect", disabled: "true", onclick: "disconnectServer", justify: "right", className: "enyo-button-affirmative"},
					{flex: 1},
					{icon: "images/keyboard.png", name: "keyboardButton", disabled: "true", onclick: "showKeyboard"},
					{icon: "images/keys.png", name: "keysButton", disabled: "true", onclick: "showKeys"}
				]}
			]}
		]}
	],
	pluginReady: false,

	create: function() {
		this.inherited(arguments);
		this.$.plugin.addCallback("pluginError", enyo.bind(this, "pluginError"), true);
	
		this.serverList = localStorage.getItem("serverList");

		if (this.serverList == undefined) {
			this.serverList = [];
		} else {
			this.serverList = JSON.parse(this.serverList);
			this.$.serverList.render();
		}

		enyo.application.bKeysButtonFirst = true;
		enyo.application.bConnected = false;
	},
	getServer: function(inSender, inIndex) {
		var r = this.serverList[inIndex];

		if (r) {
			// Check to see if the row is selected.
			var isRowSelected = (inIndex == this.selectedRow);

			this.$.listItem.applyStyle("background", isRowSelected ? "yellow" : null);

			this.$.listItemTitle.setContent(r.title);
			return true;
		}
	},
	serverClick: function(inSender, inEvent) {
		this.selectedRow = inEvent.rowIndex;
		this.$.serverList.refresh();
		this.$.editButton.setDisabled(false);
		this.enableButtons();
	},
	saveServerList: function() {
		localStorage.setItem("serverList", JSON.stringify(this.serverList));
	},
	showAddNewServerPopup: function() {
		this.$.newServerDialog.openAtCenter();
	},
	showEditServerPopup: function() {
		this.$.editServerDialog.openAtCenter();
		var r = this.serverList[this.selectedRow];
		this.$.editServerTitle.setValue(r.title);
		this.$.editServerURL.setValue(r.url);
		this.$.editServerPort.setValue(r.port);
		this.$.editServerPassword.setValue(r.password);
	},
	addServer: function() {
		this.serverList.push({title: this.$.newServerTitle.getValue(), url: this.$.newServerURL.getValue(), port: this.$.newServerPort.getValue(), password: this.$.newServerPassword.getValue()});
		this.resetNewServerData();
		this.saveServerList();
		this.$.serverList.refresh();
	},
	editServer: function() {
		this.serverList[this.selectedRow] = {title: this.$.editServerTitle.getValue(), url: this.$.editServerURL.getValue(), port: this.$.editServerPort.getValue(), password: this.$.editServerPassword.getValue()};
		this.resetEditServerData();
		this.saveServerList();
		this.$.serverList.refresh();
	},
	cancelAdd: function() {
		this.$.newServerDialog.close();
		this.resetNewServerData();
	},
	cancelEdit: function() {
		this.$.editServerDialog.close();
		this.resetEditServerData();
	},
	resetNewServerData: function() {
		this.$.newServerTitle.setValue("");
		this.$.newServerURL.setValue("");
		this.$.newServerPort.setValue("5900");
		this.$.newServerPassword.setValue("");
		this.$.newServerDialog.close();
	},
	resetEditServerData: function() {
		this.$.editServerTitle.setValue("");
		this.$.editServerURL.setValue("");
		this.$.editServerPort.setValue("5900");
		this.$.editServerPassword.setValue("");
		this.$.editServerDialog.close();
	},
	deleteServerItem: function(inSender, inIndex) {
		this.serverList.splice(inIndex, 1);
		this.saveServerList();
		this.$.serverList.refresh();
	},
	handlePluginReady: function(inSender) {
		console.log("plugin initialized");
		this.pluginReady = true;
		console.log("plugin ready");
	},
	resizePlugin: function() {
		if(enyo.application.bConnected)
		{
			var w = 0;
			var h = 0;

			if(enyo.getWindowOrientation() == "left" | enyo.getWindowOrientation() == "right") {
				var w = this.$.VncViewPaneBar.hasNode().clientWidth;
				var h = 936;	
			}
			else {
				var w = this.$.VncViewPaneBar.hasNode().clientWidth;
				var h = 680;
			}

			this.$.plugin.setWidth(w+8);
			this.$.plugin.setHeight(h);
		//	this.$.generalPopup.setContent("width set to:" + w + ", height set to: " + h);
		//	this.$.generalPopup.openAtCenter();
		}
		else
		{
			this.$.plugin.setWidth(1);
			this.$.plugin.setHeight(1);
		}
	},
/*	windowRotated: function(inSender) {
		if(enyo.getWindowOrientation() == "up" OR enyo.getWindowOrientation() == "down") {
			this.$.left.setShowing(false);
		}
		else if(enyo.getWindowOrientation() == "left" OR enyo.getWindowOrientation() == "right") {
			this.$.left.setShowing(true);
		}
	},*/
	pluginFail: function() {
		enyo.application.bConnected = false;
		this.resizePlugin();
		this.enableButtons();

		this.$.generalPopup.setContent("Plugin has stopped working. Restart application");
		this.$.generalPopup.openAtCenter();
	},
	pluginError: function(param) {
		enyo.application.bConnected = false;
		this.resizePlugin();
		this.enableButtons();

		this.$.generalPopup.setContent(param);
		this.$.generalPopup.openAtCenter();
	},
	connectServer: function() {
		if(this.pluginReady) {
			enyo.application.bConnected = true;
			this.resizePlugin();
			this.enableButtons();

			var r = this.serverList[this.selectedRow];
			this.$.plugin.callPluginMethod("connect", r.url, r.port, r.password);
		}
	},
	disconnectServer: function() {
		if(this.pluginReady) {
			this.$.plugin.callPluginMethod("disconnect");
			enyo.application.bConnected = false;
			this.resizePlugin();
			this.enableButtons();
		}
	},
	enableButtons: function() {
		if(enyo.application.bConnected)
		{
			this.$.keysButton.setDisabled(false);
			this.$.keyboardButton.setDisabled(false);
			this.$.disconnectButton.setDisabled(false);
			this.$.connectButton.setDisabled(true);
		}
		else {
			this.$.keysButton.setDisabled(true);
			this.$.keyboardButton.setDisabled(true);
			this.$.disconnectButton.setDisabled(true);
			this.$.connectButton.setDisabled(false);
		}
	},
	showKeys: function() {
		if( enyo.application.bKeysButtonFirst )
		{
			var inRect = {bottom: 45, right: 0};
			this.$.popupKeys.openAt(inRect);
			enyo.application.bKeysButtonFirst = false;
		}
		else this.$.popupKeys.toggleOpen();
	},
	keySelect: function(inSender, inSelected) {
		if(this.pluginReady) {
			var v = inSelected.getValue();
			if(v == "up") this.$.plugin.callPluginMethod("keypress",0xff52);
			else if(v == "down") this.$.plugin.callPluginMethod("keypress",0xff54);
			else if(v == "left") this.$.plugin.callPluginMethod("keypress",0xff51);
			else if(v == "right") this.$.plugin.callPluginMethod("keypress",0xff53);
			else if(v == "win") this.$.plugin.callPluginMethod("keypress",0xffe7);
			else if(v == "del") this.$.plugin.callPluginMethod("keypress",0xffff);
			else if(v == "home") this.$.plugin.callPluginMethod("keypress",0xff50);
			else if(v == "end") this.$.plugin.callPluginMethod("keypress",0xff57);
			else if(v == "pgup") this.$.plugin.callPluginMethod("keypress",0xff55);
			else if(v == "pgdn") this.$.plugin.callPluginMethod("keypress",0xff56);
			else if(v == "esc") this.$.plugin.callPluginMethod("keypress",0xff1b);
			
			this.$.popupKeys.open();
		}
	},
	showKeyboard: function() {
		if(this.pluginReady) {
			this.$.plugin.focus();
		}
	},
	showAboutContent: function(inSender) {
		this.$.generalPopup.setContent("junkVNC, developed by John Kyrus. Contact me at jnk5y@yahoo.com for source code or bug reports.<br><br>A big 'Thank You' to Bernard Slawik who gave me his code for VNC Client PDK.<br>VNC Client PDK (C) 2010 Bernhard Slawik Digital, <a href='http://www.bernhardslawik.de'>http://www.bernhardslawik.de</a><br><br>Based on SDL_vnc by A. Schiffler (LGPL, <a href='http://sourceforge.net/projects/sdlvnc/'>http://sourceforge.net/projects/sdlvnc/</a>, aschiffler at ferzkopp dot net)");
		this.$.generalPopup.openAtCenter();
	}
});
