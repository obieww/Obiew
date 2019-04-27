const utils = {};

utils.defaultUrl = "https://sleepy-island-43632.herokuapp.com"

utils.defaultObiew = strMsg => ({
    userId: Date.now(),
    obiewId: Date.now(),
    pic: "../../images/person_default.png",
    username: "Anonymous",
    timestamp: Date.now(),
    hasReobiewed: false,
    reobiews: [],
    likes: [], 
    comments: [], 
    obiew: strMsg
});

utils.timeFormat = item => {
  let time = new Date(item);
  let month = ((mon) => {
    switch (mon) {
      case 1: return 'Jan';
      case 2: return 'Feb';
      case 3: return 'Mar';
      case 4: return 'Apr';
      case 5: return 'May';
      case 6: return 'Jun';
      case 7: return 'Jul';
      case 8: return 'Aug';
      case 9: return 'Sep';
      case 10: return 'Oct';
      case 11: return 'Nov';
      case 12: return 'Dec';
      default: return '';
    }
  })(time.getMonth());
  return time.getDate() + ' ' + month + ' ' +  time.getFullYear();
};

export default utils;