import React, {Component} from "react";
import Logo from "@/images/logo.png";
import "./Post.less";

class Post extends Component {
  constructor(props) {
    super(props);
    this.state = {
      inputMessage: "",
    };
    this.onInputChange = this.onInputChange.bind(this);
    this.onClick = this.onClick.bind(this);
  }

  onInputChange(e) {
    this.setState({
      inputMessage: e.target.value
    });
  }

  onClick(e) {
    this.props.onClick && this.props.onClick(this.state.inputMessage);
    this.setState({
      inputMessage: ""
    });
  }

  render() {
    const {
      inputMessage
    } = this.state
    return (
      <div className="panel panel-default panel-remark-custom">
        <div className="panel-body">
          <textarea
            className="form-control input-style"
            value={inputMessage}
            placeholder={this.props.msg}
            onChange={this.onInputChange} />
        </div>
        <div className="footer-custom">
          <a className={"btn grow" + (inputMessage === "" ? " disabled" : "")}
            onClick={this.onClick}>
            <img className="logo" src={Logo} alt="post"/>
          </a>
        </div>
      </div>
    );
  }
}

export default Post;