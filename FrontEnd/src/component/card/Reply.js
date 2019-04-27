import React, {Component} from "react";
import Obiew from "./Obiew.js";

class Reply extends Component {
  render() {
    return (
      <div>
        {this.props.comments.map((comment, index) => (
          <div className="reply" key={index}>
            <Obiew info={{
              username: comment.username,
              timestamp: comment.timestamp,
              obiew: comment.reply,
              rightBtns: []}} />
          </div>
        ))}
      </div>
    )
  }
}

export default Reply