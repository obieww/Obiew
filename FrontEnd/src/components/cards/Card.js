import React, {Component} from "react";
import Obiew from "@/components/common/Obiew.js";
import Reply from "@/components/cards/Reply.js";
import Post from "@/components/common/Post.js";
import "./Card.less";

class Card extends Component {
  constructor(props) {
    super(props);
    this.state = {
      obiew: props.obiew,
      liked: false,
      commented: false,
      showComment: false,
      inputMessage: "",
      hasReobiewed: false,
    };
    this.onClickRightBtn = this.onClickRightBtn.bind(this);
    this.onInputChange = this.onInputChange.bind(this);
    this.onCreateNewReply = this.onCreateNewReply.bind(this);
  }

  onInputChange(e) {
    this.setState({
      inputMessage: e.target.value
    });
  }

  onClickRightBtn(rightBtn) {
    if (rightBtn.key === "like") {
      let likes = this.state.obiew.likes
      if (!this.state.liked) {
        likes.push({
          likeId: 100,
          userId: 100,
          obiewId: this.state.obiew.obiewId
        })
      } else {
        likes.pop();
      }
      this.setState((prevState, props) => ({
        ...prevState,
        obiew: {
          ...prevState.obiew,
          likes: likes,
        },
        liked: !prevState.liked,
      }));
    } else if (rightBtn.key === "comment") {
      this.setState((prevState, props) => ({
        ...prevState,
        commented: !prevState.commented,
        showComment: !prevState.showComment,
      }));
    } else if (rightBtn.key === 'reobiew') {
      if (!this.state.hasReobiewed) {
        let reobiews = this.state.obiew.reobiews;
        reobiews.unshift(            {
          userId: 1,
          obiewId: 1,
          timestamp: Date.now(),
        })
        this.setState((prevState, props) => ({
          ...prevState,
          obiew: {
            ...prevState.obiew,
            reobiews: reobiews,
          },
          hasReobiewed: true,
        }));
      }
    }
  }

  onCreateNewReply(strMsg) {
    let comments = this.state.obiew.comments;
    comments.unshift({
      commentId: 100,
      obiewId: this.state.obiew.obiewId,
      userId: this.state.obiew.userId,
      timestamp: Date.now(),
      reply: strMsg,
    });
    this.setState((prevState, props) => ({
      ...prevState,
      obiew: {
        ...prevState.obiew,
        comments: comments,
      }
    }))
  }

  render() {
    let obiew = {
      pic: this.state.obiew.pic,
      username: this.state.obiew.username,
      timestamp: this.state.obiew.timestamp,
      obiew: this.state.obiew.obiew,
      rightBtns: [
        {
          key: "like",
          label: this.state.obiew.likes.length,
          icon: "glyphicon-heart"
        },
        {
          key: "comment",
          label: this.state.obiew.comments.length,
          icon: "glyphicon-comment"
        },
        {
          key: "reobiew",
          label: this.state.obiew.reobiews.length,
          icon: "glyphicon-new-window"
        },
      ]
    };

    return (
      <div className="panel-default panel-custom">
        <div className="obiew">
          <Obiew info={obiew} onClickRightBtn={this.onClickRightBtn}/>
        </div>
        {
          this.state.showComment ? 
          <div className="panel-body input">
            <Post msg="Obiew your reply" onClick={this.onCreateNewReply} ref={e => this.inputArea = e}/>
            <Reply comments={this.state.obiew.comments} />
          </div> :
          null
        }
      </div>
    );
  }
}

export default Card;