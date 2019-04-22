import React, {Component} from "react";
import Obiew from "@/components/common/Obiew.js";

class Reply extends Component {
  render() {
    return (
      <div>
        {
          this.props.comments.map((comment, index) => (
            <div className="reply" key={index}>
              <Obiew info={{
                pic: "",
                username: "Anonymous",
                timestamp: comment.timestamp,
                obiew: comment.reply,
                rightBtns: []
                }} />
            </div>))
        }
      </div>
    )
  }
}

export default Reply