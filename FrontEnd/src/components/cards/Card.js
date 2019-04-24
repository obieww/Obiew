import React, {Component} from "react";
import Obiew from "@/components/common/Obiew.js";
import Reply from "@/components/cards/Reply.js";
import Post from "@/components/common/Post.js";
import "./Card.less";

class Card extends Component {
  constructor(props) {
    super(props);
    this.state = {
      card: props.card,
      liked: false,
      commented: false,
      showComment: false,
      inputMessage: "",
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
      let likes = this.state.card.likes
      if (!this.state.liked) {
        likes.push({
          likeId: 100,
          userId: 100,
          obiewId: this.state.card.obiewId
        })
      } else {
        likes.pop();
      }
      this.setState((prevState, props) => ({
        ...prevState,
        card: {
          ...prevState.card,
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
    }
  }

  onCreateNewReply(strMsg) {
    let comments = this.state.card.comments;
    comments.unshift({
      commentId: 100,
      obiewId: this.state.card.obiewId,
      userId: this.state.card.userId,
      timestamp: Date.now(),
      reply: strMsg,
    });
    this.setState((prevState, props) => ({
      ...prevState,
      card: {
        ...prevState.card,
        comments: comments,
      }
    }))
  }

  render() {
    let card = {
      pic: this.state.card.pic,
      username: this.state.card.username,
      timestamp: this.state.card.timestamp,
      obiew: this.state.card.obiew,
      rightBtns: [
        {
          key: "like",
          label: this.state.card.likes.length,
          icon: "glyphicon-heart"
        },
        {
          key: "comment",
          label: this.state.card.comments.length,
          icon: "glyphicon-comment"
        }
      ]
    };

    return (
      <div className="panel-default panel-custom">
        <div className="obiew">
          <Obiew info={card} onClickRightBtn={this.onClickRightBtn}/>
        </div>
        {
          this.state.showComment ? 
          <div className="panel-body input">
            <Post msg="Obiew your reply" onClick={this.onCreateNewReply} ref={e => this.inputArea = e}/>
            <Reply comments={this.state.card.comments} />
          </div> :
          null
        }
      </div>
    );
  }
}

export default Card;