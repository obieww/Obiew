const utils = {};

utils.defaultObiew = (cards, strMsg) => ({
    userId: cards.length + 1,
    obiewId: cards.length + 1,
    pic: "../../images/person_default.png",
    username: "Anonymous",
    timestamp: Date.now(),
    likes: [], 
    comments: [], 
    obiew: strMsg
})

utils.defaultObiews = [
    {
        userId: 0,
        obiewId: 0,
        pic: "../../images/person_default.png",
        username: "Anonymous",
        timestamp: Date.now(),
        likes: [
            {
                userId: 1,
                likeId: 0,
                obiewId: 0
            },
            {
                userId: 2,
                likeId: 1,
                obiewId: 0
            },
            {
                userId: 3,
                likeId: 2,
                obiewId: 0
            },
        ], 
        comments: [
            {
                commentId: 0,
                obiewId: 0,
                userId: 1,
                timestamp: Date.now(),
                reply: "Words cannot describe the tremendous sorrow for the great loss of the innocents."
            },
            {
                commentId: 1,
                obiewId: 0,
                userId: 2,
                timestamp: Date.now(),
                reply: "Words cannot describe the tremendous sorrow for the great loss of the innocents."
            }
        ], 
        obiew: "The number of people who died in the Easter Sunday attacks in Sri Lanka has risen sharply to 290, police say."
    },
    {
        userId: 2,
        obiewId: 1,
        pic: "../../images/person_default.png",
        username: "Anonymous",
        timestamp: Date.now(),
        likes: [
            {
                userId: 1,
                likeId: 3,
                obiewId: 1,
            }
        ], 
        comments: [
            {
                commentId: 2,
                obiewId: 1,
                userId: 1,
                timestamp: Date.now(),
                reply: "Words cannot describe the tremendous sorrow for the great loss of the innocents."
            }
        ], 
        obiew: "The number of people who died in the Easter Sunday attacks in Sri Lanka has risen sharply to 290, police say."
    }
]

utils.timeFormat = item => {
    let time = new Date(item);
    let month = ((mon) => {
        switch (mon) {
            case 1: {
                return 'Jan';
            }
            case 2: {
                return 'Feb';
            }
            case 3: {
                return 'Mar';
            }
            case 4: {
                return 'Apr';
            }
            case 5: {
                return 'May';
            }
            case 6: {
                return 'Jun';
            }
            case 7: {
                return 'Jul';
            }
            case 8: {
                return 'Aug';
            }
            case 9: {
                return 'Sep';
            }
            case 10: {
                return 'Oct';
            }
            case 11: {
                return 'Nov';
            }
            case 12: {
                return 'Dec';
            }
            default: {
                return '';
            }
        }
    })(time.getMonth());
    return time.getDate() + ' ' + month + ' ' +  time.getFullYear();
};

utils.insert4String = function (strSrc, index, strInsert) {
    return strSrc.slice(0, index) + strInsert + strSrc.slice(index);
};

export default utils;