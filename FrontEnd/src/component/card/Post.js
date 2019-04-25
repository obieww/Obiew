import React, {Component} from "react";
import Logo from "../../images/icon.png";
import "./Post.less";

class Post extends Component {
  constructor(props) {
    super(props);
    this.state = {
      inputMessage: "",
    };
    this.onInputChange = this.onInputChange.bind(this);
  }

  onInputChange(e) {
    this.setState({
      inputMessage: e.target.value
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
            onChange={e => this.setState({ inputMessage: e.target.value})} />
        </div>
        <div className="footer-custom">
          <div
            className={"btn grow" + (inputMessage === "" ? " disabled" : "")}
            onClick={() => this.props.onClick(this.state.inputMessage)}>
            <img className="logo" src={Logo} alt="post"/>
          </div>
        </div>
      </div>
    );
  }
}

export default Post;