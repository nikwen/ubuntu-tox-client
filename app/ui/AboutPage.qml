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

Page {
    id: aboutPage
    title: i18n.tr("About ubuntu-tox-client")
    visible:false

    Flickable {
        id: flickable
        anchors.fill: parent
        clip: true

        contentHeight: aboutColumn.height + aboutColumn.marginTop

        Column {
            id: aboutColumn
            width: parent.width
            property real marginTop: units.gu(3)
            y: marginTop

            UbuntuShape {
                property real maxWidth: units.gu(45)
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(parent.width, maxWidth)/2
                height: Math.min(parent.width, maxWidth)/2
                image: Image {
                    source: "../graphics/ubuntu-tox-client.png"
                    smooth: true
                    fillMode: Image.PreserveAspectFit
                }
            }

            Item {
                id: spacer
                width: parent.width
                height: units.gu(2)
            }

            ListItem.Header {
                text: i18n.tr("Info:")
            }

            ListItem.Standard {
                text: i18n.tr("Version:")
                control: Label {
                    text: "0.0.2"
                }
            }

            ListItem.Header {
                text: i18n.tr("Development:")
            }

            ListItem.Standard {
                text: i18n.tr("License:")
                control: Label {
                    text: "GPL v3"
                }
                progression: true
                onClicked: Qt.openUrlExternally("http://www.gnu.org/licenses/gpl-3.0.txt")
            }

            ListItem.Standard {
                text: i18n.tr("Source code & bug tracker:")
                control: Label {
                    text: "Github"
                }
                progression: true
                onClicked: Qt.openUrlExternally("https://github.com/nikwen/ubuntu-tox-client")
            }

            ListItem.Header {
                text: i18n.tr("Authors:")
            }

            ListItem.Standard {
                text: "Niklas Wenzel"
                control: Label {
                    text: i18n.tr("Maintainer:")
                }
            }

            ListItem.Header {
                text: i18n.tr("Contact:")
            }

            ListItem.Standard {
                text: "nikwen.developer@gmail.com"
                progression: true
                onClicked: Qt.openUrlExternally("mailto:nikwen.developer@gmail.com")
            }

            ListItem.Header {
                text: i18n.tr("Uses the following libraries:")
            }

            ListItem.Standard {
                text: "toxcore"
                control: Label {
                    text: "GPL v3"
                }
                progression: true
                onClicked: Qt.openUrlExternally("https://github.com/irungentoo/toxcore")
            }

            ListItem.Standard {
                text: "libsodium"
                control: Label {
                    text: i18n.tr("ISC license")
                }
                progression: true
                onClicked: Qt.openUrlExternally("https://github.com/jedisct1/libsodium")
            }

            ListItem.Header {
                text: i18n.tr("Uses code from the following projects:")
            }

            ListItem.Standard {
                text: "qTox"
                control: Label {
                    text: "GPL v3"
                }
                progression: true
                onClicked: Qt.openUrlExternally("https://github.com/tux3/qTox")
            }

            ListItem.Standard {
                text: "uTox"
                control: Label {
                    text: "GPL v3"
                }
                progression: true
                onClicked: Qt.openUrlExternally("https://github.com/notsecure/uTox")
            }

            ListItem.Empty {
                id: toxWebsiteItem
                width: parent.width
                height: units.gu(9)
                divider.visible: false

                onClicked: Qt.openUrlExternally("https://tox.im")

                Image {
                    height: units.gu(6)
                    anchors.centerIn: parent
                    source: "../graphics/tox_logo.png" //TODO: svg file?
                    smooth: true
                    fillMode: Image.PreserveAspectFit
                }
            }
        }
    }
}
