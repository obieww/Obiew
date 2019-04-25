import React, {Component} from "react";
import Post from "./card/Post.js";
import Card from "../container/CardContainer.js";
import "./MainPanel.less";

class MainPanel extends Component {
  constructor(props) {
    super(props);
    this.state = {
      // hasNewMsg: true,
      showAnchor: false,
    };
    // this.onShowNewMessage = this.onShowNewMessage.bind(this);
    this.onMousewheel = this.onMousewheel.bind(this);
    this.onCreateNewObiew = this.onCreateNewObiew.bind(this);
  }

  componentWillMount() {
    window.addEventListener("scroll", this.onMousewheel);
  }

  componentWillUnmount() {
    window.removeEventListener("scroll", this.onMousewheel);
  }

  onMousewheel() {
    this.setState({
      showAnchor: window.scrollY >= 50
    });
  }

  // onShowNewMessage(e) {
  //   let obiews = this.props.obiews.slice(0);
  //   obiews.unshift(utils.defaultObiew(obiews, "new message"))
  //   this.setState({
  //     obiews: obiews
  //   })
  //   this.setState({
  //     hasNewMsg: false
  //   });
  //   window.scrollTo(0, 0);
  // }

  onCreateNewObiew(msg) {
    const {
      obiews,
      onCreateNewObiew,
    } = this.props
    onCreateNewObiew({
      userId: obiews.length + 1,
      obiewId: obiews.length + 1,
      pic: "../../images/person_default.png",
      username: "Anonymous",
      timestamp: Date.now(),
      hasReobiewed: false,
      reobiews: [],
      likes: [], 
      comments: [], 
      obiew: msg,
    });
    this.setState({
      inputMessage: ""
    });
  }

  render() {
    const {
      showAnchor,
    } = this.state;
    const {
      obiews,
    } = this.props;
    return (
      <div className="main-content row col-md-8">
        <Post
          msg="Tell the world what's happening"
          onClick={this.onCreateNewObiew} />
        {/*
          this.state.hasNewMsg ?
          <div className='panel-info panel-heading new-info' onClick={this.onShowNewMessage}>
            <span>See more obiews</span>
          </div> :
          null
        */}
        {
          obiews.map(obiew => <Card key={obiew.obiewId} obiew={obiew} />)
        }
        {
          showAnchor ?
          <div
            className="anchor" 
            v-show="showAnchor"
            onClick={() => window.scrollTo(0, 0)}>
            <span className="glyphicon glyphicon-chevron-up" />
          </div> :
          null
        }
      </div>
    );
  }
}

export default MainPanel;