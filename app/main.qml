/*
 * ubuntu-tox-client
 *
 * Copyright (c) 2015 Niklas Wenzel <nikwen.developer@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Themes.Ambiance 0.1
import UbuntuToxClient 1.0

import "ui"

MainView {
    applicationName: "com.ubuntu.developer.nikwen.ubuntu-tox-client"

    useDeprecatedToolbar: false

    width: units.gu(100)
    height: units.gu(75)

    PageStack {
        id: pageStack

        Component.onCompleted: push(startPage)
    }

    Page {
        id: startPage
        visible: false

        title: i18n.tr("Tox-Client")

        head.actions: [
            Action {
                id: contacts1Action
                iconName: "contact"
                text: i18n.tr("Contacts")

                onTriggered: pageStack.push(contactsPage)
            },
            Action {
                id: aboutAction
                iconName: "info"
                text: i18n.tr("About")

                onTriggered: pageStack.push(aboutPage)
            },
            Action {
                id: settingsAction
                iconName: "settings"
                text: i18n.tr("Settings")

                onTriggered: pageStack.push(settingsPage)
            }
        ]

        Column {
            anchors {
                fill: parent
                margins: units.gu(2)
            }
            spacing: units.gu(2)

            Label {
                id: toxIdLabel
                width: parent.width
                wrapMode: Text.Wrap

                text: i18n.tr("Your Tox-ID is (click to copy):") + (copyCheckMarkTimer.running ? " ✓<br>" : "<br>") + backend.toxId

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        var mimeData = Clipboard.newData()
                        mimeData.text = backend.toxId
                        Clipboard.push(mimeData)

                        copyCheckMarkTimer.restart()

                        console.log("Copied Tox-ID to clipboard")
                    }
                }

                Timer {
                    id: copyCheckMarkTimer
                    interval: 2000
                    repeat: false
                }
            }

            Label {
                id: connectedLabel
                width: parent.width
                wrapMode: Text.Wrap

                text: backend.connected ? i18n.tr("You are connected to the DHT") : i18n.tr("You are <b>not</b> connected to the DHT")
            }

            Label { //TODO: Better solution for friendship requests!
                id: friendRequestLabel
                width: parent.width
                wrapMode: Text.Wrap

                property string userId: ""
                property string message: ""

                text: (userId !== "") ? qsTr(i18n.tr("Incoming friend request from user %1<br>Message: %2")).arg(userId).arg(message) : ""
                visible: text !== ""
            }

            Button {
                id: acceptButton
                width: parent.width
                visible: friendRequestLabel.visible
                color: UbuntuColors.green

                text: i18n.tr("Accept friend request")

                onClicked: backend.acceptFriendRequest(friendRequestLabel.userId)
            }

            Label {
                id: addFriendResultLabel
                width: parent.width
                wrapMode: Text.Wrap

                property bool success: false
                property string userId: ""

                text: (success === true) ? i18n.tr("Added friend successfully! :)") : i18n.tr("Failed to add friend! :(")
                visible: userId !== ""
            }
        }
    }

    Page {
        id: contactsPage
        title: i18n.tr("Contacts")
        visible: false

        head.actions: [
            Action {
                id: addFriendAction
                iconName: "add"
                text: "Add friend"

                onTriggered: pageStack.push(addFriendPage)
            }
        ]

        ListView {
            id: contactsListView
            height: parent.height
            width: parent.width
            clip: true

            model: ContactsModel {
                toxBackend: backend
            }

            delegate: ListItem.Subtitled {
                text: model.name
                subText: model.statusMessage
                progression: true
                onClicked: {
                    pageStack.push(contactsDetailsPage)
                    contactsDetailsPage.headline=model.name
                }
            }
        }
    }

    Page {
        id: addFriendPage
        visible: false

        title: i18n.tr("Add a friend")

        head.actions: [
            Action {
                id: confirmAddFriendAction
                iconName: "ok"
                text: i18n.tr("Add friend")

                onTriggered: {
                    var message = (friendMessageTextField.text.length > 0) ? friendMessageTextField.text : friendMessageTextField.placeholderText
                    backend.sendFriendRequest(friendAddressTextField.text, message)
                }
            }
        ]

        head.backAction: Action {
            id: cancelAddFriendAction
            iconName: "close"
            text: i18n.tr("Cancel")

            onTriggered: {
                //Clear text fields
                friendAddressTextField.text = ""
                friendMessageTextField.text = ""

                pageStack.pop()
            }
        }

        Column {
            anchors.fill: parent
            spacing: units.gu(1)

            ListItem.Header {
                text: i18n.tr("Tox ID:")
            }

            TextField {
                id: friendAddressTextField
                maximumLength: backend.getFriendAddressSize()
                placeholderText: i18n.tr("Your friend's Tox ID")

                anchors {
                    right: parent.right
                    left: parent.left
                    rightMargin: units.gu(2)
                    leftMargin: units.gu(2)
                }

                style: TextFieldStyle {
                    background: Item {}
                }

                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: friendMessageTextField
            }

            ListItem.Header {
                text: i18n.tr("Message:")
            }

            TextField {
                id: friendMessageTextField
                maximumLength: backend.getMaximumFriendRequestLength()
                placeholderText: i18n.tr("Let's tox!")

                anchors {
                    right: parent.right
                    left: parent.left
                    rightMargin: units.gu(2)
                    leftMargin: units.gu(2)
                }

                style: TextFieldStyle {
                    background: Item {}
                }

                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: friendAddressTextField
            }

            Connections {
                target: backend

                onFriendAdded: {
                    if (pageStack.currentPage === addFriendPage) {
                        //Clear text fields
                        friendAddressTextField.text = ""
                        friendMessageTextField.text = ""

                        pageStack.pop()
                    }
                }

                onFailedToAddFriend: {
                    if (pageStack.currentPage === addFriendPage) {
                        console.log("Error while adding friend:", errorCode)
                        var dialog = PopupUtils.open(addFriendErrorDialog)
                        dialog.errorCode = errorCode
                    }
                }
            }
        }

        Component {
             id: addFriendErrorDialog

             Dialog {
                 id: dialog
                 title: getTitleForErrorCode(errorCode)
                 text: getTextForErrorCode(errorCode)

                 property int errorCode: 0

                 onErrorCodeChanged: console.log(errorCode)

                 function getTitleForErrorCode(errorCode) {
                     if (errorCode === backend.getFAErrSetNewNospam()) {
                         return i18n.tr("Updated nospam value")
                     } else {
                         return i18n.tr("Failed to add friend")
                     }
                 }

                 function getTextForErrorCode(errorCode) {
                     switch (errorCode) {
                     case backend.getFAErrTooLong():
                         return i18n.tr("The entered message is too long!")
                     case backend.getFAErrNoMessage():
                         return i18n.tr("No message has been entered!")
                     case backend.getFAErrOwnKey():
                         return i18n.tr("You cannot add yourself as a friend! ;)")
                     case backend.getFAErrAlreadySent():
                         return i18n.tr("You have already sent that user a friendship request!")
                     case backend.getFAErrUnknown():
                         return i18n.tr("Unknown error...")
                     case backend.getFAErrBadChecksum():
                         return i18n.tr("There is a checksum error in the entered Tox ID!")
                     case backend.getFAErrSetNewNospam():
                         return i18n.tr("The nospam value has been updated!")
                     case backend.getFAErrNoMem():
                         return i18n.tr("Increasing the friend list size has failed!")
                     default:
                         return ""
                     }
                 }

                 Button {
                     text: i18n.tr("OK")
                     color: UbuntuColors.green
                     onClicked: {
                         PopupUtils.close(dialog)
                         if (errorCode === backend.getFAErrSetNewNospam()) {
                             pageStack.pop()
                         }
                     }
                 }
             }
        }
    }

    Page {
        id: contactsDetailsPage
        title: i18n.tr("Chat with ")+headline
        visible: false
        anchors.fill: parent
        property string headline: ""

                ListModel {
                id: exampleListModel

                ListElement {
                    sender: "person A"
                    name: "Hey"
                    timestamp: "17:30:34"
                    }
                ListElement {
                    sender: "Person B"
                    name: "How are you?"
                    timestamp: "17:35:30"
                    }
                ListElement {
                    sender: "Person A"
                    name: "I'm fine."
                    timestamp: "17:39:04"
                    }
                ListElement {
                    sender: "Person A"
                    name: "I'm fine."
                    timestamp: "17:39:04"
                    }
                ListElement {
                    sender: "Person A"
                    name: "I'm fine."
                    timestamp: "17:39:04"
                    }
                ListElement {
                    sender: "Person A"
                    name: "I'm fine."
                    timestamp: "17:39:04"
                    }
                ListElement {
                    sender: "Person A"
                    name: "I'm fine."
                    timestamp: "17:39:04"
                    }

            }

            ListView {
                id: exampleListView

                height: parent.height-messageColumn.height-units.gu(5)
                width: parent.width
                model: exampleListModel
                anchors {
                    bottom: messageColumn.top
                    top: contactsDetailsPage.top
                }
              delegate: UbuntuShape {
                          color: "lightgrey"
                          height: timestampLabel.height+textLabel.height+units.gu(2)
                          width: parent.width

                          Row {
                              id: headerRow
                              spacing: units.gu(3)

                            Label {
                                id: timestampLabel
                                text: timestamp
                                wrapMode: Text.WordWrap
                            }
                            Label {
                                id: senderLabel
                                text: sender
                                wrapMode: Text.WordWrap
                            }
                          }

                          Label {
                              id: textLabel
                              text: name
                              wrapMode: Text.WordWrap
                              anchors {
                                  top: headerRow.bottom
                              }

                         }
                }
             }

            Column {
                id: messageColumn
                width: parent.width
                spacing: units.gu(2)
                anchors {
                    bottom: parent.bottom
                }

                ListItem.Header {
                    text: i18n.tr("New Message:")
                }

                Row {

                    id: messageRow
                    width: messageColumn.width
                    spacing: units.gu(2)
                    anchors.margins: units.gu(2)


                    TextArea {
                        id: newMessageTextField
                        placeholderText: i18n.tr("Your message")
                        width: messageRow.width-units.gu(8)
                        height: units.gu(20)

                        style: TextFieldStyle {
                            color: "grey"
                        }

                        KeyNavigation.priority: KeyNavigation.BeforeItem
                        KeyNavigation.tab: newMessageTextField
                    }

                    Button {
                        id: sendOwnMessageButton
                        color: UbuntuColors.green
                        iconName: "ok"
                        height:units.gu(3)
                        width: units.gu(3)

                    }
                }
            }


       }




    Page {
        id: settingsPage
        visible: false

        title: i18n.tr("Settings")

        Component.onCompleted: updateLabels()

        head.actions: [
            Action {
                id: saveAction
                iconName: "ok"
                text: i18n.tr("Save")

                onTriggered: {
                    backend.setUserName(userNameTextField.text)
                    backend.setStatusMessage(statusMessageTextField.text)
                    pageStack.pop()
                }
            }
        ]

        head.backAction: Action {
            id: cancelAction
            iconName: "close"
            text: i18n.tr("Cancel")

            onTriggered: {
                settingsPage.updateLabels()
                pageStack.pop()
            }
        }

        function updateLabels() {
            userNameTextField.text = backend.userName
            statusMessageTextField.text = backend.statusMessage
        }

        Connections {
            target: backend
            onUserNameChanged: userNameTextField.text = backend.userName
            onStatusMessageChanged: statusMessageTextField.text = backend.statusMessage
        }

        Column {
            anchors.fill: parent
            spacing: units.gu(1)

            ListItem.Header {
                text: i18n.tr("User name:")
            }

            TextField {
                id: userNameTextField
                maximumLength: backend.getMaximumUserNameLength()

                anchors {
                    right: parent.right
                    left: parent.left
                    rightMargin: units.gu(2)
                    leftMargin: units.gu(2)
                }

                style: TextFieldStyle {
                    background: Item {}
                }

                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.tab: statusMessageTextField
            }

            ListItem.Header {
                text: i18n.tr("Status message:")
            }

            TextField {
                id: statusMessageTextField
                maximumLength: backend.getMaximumStatusMessageLength()

                anchors {
                    right: parent.right
                    left: parent.left
                    rightMargin: units.gu(2)
                    leftMargin: units.gu(2)
                }

                style: TextFieldStyle {
                    background: Item {}
                }

                KeyNavigation.priority: KeyNavigation.BeforeItem
                KeyNavigation.backtab: userNameTextField
            }
        }
    }

    AboutPage {
        id: aboutPage
        visible: false
    }

    ToxBackend {
        id: backend

        onConnectedChanged: console.log(connected ? "Connected" : "Disconnected")

        Component.onDestruction: {
            console.log("ToxBackend: Component.onDestruction received")
            backend.cleanUpOnClose()
        }

        onFriendRequestReceived: {
            console.log("Incoming friend request :)")
            friendRequestLabel.userId = userId
            friendRequestLabel.message = message
            addFriendResultLabel.userId = ""
            addFriendResultLabel.success = false
        }

        onFriendAdded: {
            friendRequestLabel.userId = ""
            friendRequestLabel.message = ""
            addFriendResultLabel.userId = userId
            addFriendResultLabel.success = true
        }

        onFailedToAddFriend: {
            friendRequestLabel.userId = ""
            friendRequestLabel.message = ""
            addFriendResultLabel.userId = userId
            addFriendResultLabel.success = false
        }
    }
}

