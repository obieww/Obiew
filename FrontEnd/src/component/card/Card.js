import React, {Component} from "react";
import utils from "../../utils";
import Obiew from "./Obiew.js";
import Reply from "./Reply.js";
import Post from "./Post.js";
import "./Card.less";

class Card extends Component {
  constructor(props) {
    super(props);
    this.state = {
      obiew: props.obiew,
      liked: false,
      showComment: false,
      hasReobiewed: false,
    };
    this.onClickRightBtn = this.onClickRightBtn.bind(this);
    this.onCreateNewReply = this.onCreateNewReply.bind(this);
  }

  onClickRightBtn(key) {
    const {
      onCreateNewLike,
      onDeleteLike,
      userId,
    } = this.props;
    const {
      obiew,
      liked,
      showComment,
      hasReobiewed,
    } = this.state
    if (key === "like") {
      if (!liked) {
        fetch(utils.defaultUrl + '/api/obiew/like', {
          method: 'post',
          headers: {'Content-Type': 'application/json'},
          body: JSON.stringify({
            user: {
              userId: userId
            },
            obiew: {
              obiewId: obiew.obiewId
            }
          })
        })
        .then(response => response.json())
        .then(response => {
          onCreateNewLike(obiew.obiewId, {
            userId: 0,
            obiewId: obiew.obiewId
          })
        })
        .catch(err => console.log(err));
      } else {
        onDeleteLike(obiew.obiewId, 0);
      }
      this.setState({
        liked: !liked,
      });
    } else if (key === "comment") {
      this.setState({
        showComment: !showComment,
      });
    } else if (key === 'reobiew') {
      if (!hasReobiewed) {
        // reobiew
      }
    }
  }

  onCreateNewReply(msg) {
    const {
      userId,
      username,
      onCreateNewComment
    } = this.props;
    const {
      obiew
    } = this.state;
    console.log(userId, obiew.obiewId);
    fetch(utils.defaultUrl + '/api/obiew/comment', {
      method: 'post',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({
        content: msg,
        user: {
          userId: userId
        },
        parent: {
          obiewId: obiew.obiewId
        }
      })
    })
    .then(response => response.json())
    .then(response => {
      onCreateNewComment(obiew.obiewId, {
        username: username,
        commentId: response.obiewId,
        obiewId: obiew.obiewId,
        userId: obiew.userId,
        timestamp: Date.now(),
        reply: msg,
      });
    })
    .catch(err => console.log(err));
  }

  render() {
    const {
      obiew,
    } = this.state;
    let config = {
      pic: obiew.pic,
      username: obiew.username,
      timestamp: obiew.timestamp,
      obiew: obiew.obiew,
      rightBtns: [
        {
          key: "like",
          label: obiew.likes.length,
          icon: "glyphicon-heart"
        },
        {
          key: "comment",
          label: obiew.comments.length,
          icon: "glyphicon-comment"
        },
        {
          key: "reobiew",
          label: obiew.reobiews.length,
          icon: "glyphicon-new-window"
        },
      ]
    };

    return (
      <div className="panel-default panel-custom">
        <div className="obiew">
          <Obiew info={config} onClickRightBtn={this.onClickRightBtn}/>
        </div>
        {
          this.state.showComment ? 
          <div className="panel-body input">
            <Post
              msg="Obiew your reply"
              onClick={this.onCreateNewReply} />
            <Reply comments={obiew.comments} />
          </div> :
          null
        }
      </div>
    );
  }
}

export default Card;