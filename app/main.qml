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
import Ubuntu.Components.ListItems 1.0 as ListItem
import Ubuntu.Components.Themes.Ambiance 0.1
import Ubuntu_Tox_Client 1.0

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

            Label {
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

        onFriendRequestReceived: {
            console.log("Incoming friend request :)")
            console.log("User-ID:", userId)
            friendRequestLabel.userId = userId
            friendRequestLabel.message = message
            addFriendResultLabel.userId = ""
            addFriendResultLabel.success = false
        }

        onFriendAdded: {
            console.log("Added friend :)")
            console.log("Friend-ID:", friendId)
            console.log("User-ID:", userId)

            friendRequestLabel.userId = ""
            friendRequestLabel.message = ""
            addFriendResultLabel.userId = userId
            addFriendResultLabel.success = true
        }

        onFailedToAddFriend: {
            console.log("Failed to add friend :(")
            console.log("User-ID:", userId)

            friendRequestLabel.userId = ""
            friendRequestLabel.message = ""
            addFriendResultLabel.userId = userId
            addFriendResultLabel.success = false
        }
    }
}

