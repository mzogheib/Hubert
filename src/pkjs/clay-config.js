module.exports = [
    {
        "type": "heading",
        "defaultValue": "LOL Time Configuration"
    },
    {
        "type": "section",
        "capabilities": ["BW"],
        "items": [
            {
                "type": "text",
                "defaultValue": "No settings for B&W Pebbles!"
            },
            {
                "type": "submit",
                "defaultValue": "Close"
            }
        ]
    },
    {
        "type": "section",
        "capabilities": ["COLOR"],
        "items": [
            {
                "type": "input",
                "messageKey": "MESSAGE",
                "defaultValue": "",
                "label": "Enter a message",
                "attributes": {
                    "placeholder": "Merely immenent",
                    "maxlength": 30,
                    "type": "text"
                }
            },
            {
                "type": "submit",
                "defaultValue": "Save Settings"
            }
        ]
    }
];