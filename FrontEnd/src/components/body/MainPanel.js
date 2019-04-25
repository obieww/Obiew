import React, {Component} from "react";
import Post from "@/components/common/Post.js";
import Card from "@/components/cards/Card.js";
import utils from "@/utils";
import "./MainPanel.less";

class MainPanel extends Component {
  constructor(props) {
    super(props);
    this.state = {
      username: 'Anonymous',
      hasNewMsg: true,
      showAnchor: false,
      obiews: []
    };
    this.onShowNewMessage = this.onShowNewMessage.bind(this);
    this.onMousewheel = this.onMousewheel.bind(this);
    this.onCreateNewObiew = this.onCreateNewObiew.bind(this);
  }

  componentWillMount() {
    this.setState({
      obiews: utils.defaultObiews,
    });
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

  onShowNewMessage(e) {
    let obiews = this.state.obiews.slice(0);
    obiews.unshift(utils.defaultObiew(obiews, "new message"))
    this.setState({
      obiews: obiews
    })
    this.setState({
      hasNewMsg: false
    });
    window.scrollTo(0, 0);
  }

  onCreateNewObiew(strMsg) {
    let obiews = this.state.obiews.slice(0);
    obiews.unshift(utils.defaultObiew(obiews, strMsg));
    this.setState({
      obiews: obiews,
    })
  }

  render() {
    const {
      obiews,
      showAnchor
    } = this.state
    return (
      <div className="main-content row col-md-8">
        <Post msg="Tell the world what's happening" onClick={this.onCreateNewObiew} ref={e => this.inputArea = e}/>
        {
          this.state.hasNewMsg ?
          <div className='panel-info panel-heading new-info' onClick={this.onShowNewMessage}>
            <span>See more obiews</span>
          </div> :
          null
        }
        {
          obiews.map(obiew=> <Card key={obiew.obiewId} obiew={obiew} />)
        }
        {
          showAnchor ?
          <a 
            className="anchor" 
            v-show="showAnchor"
            onClick={() => window.scrollTo(0, 0)}>
            <span className="glyphicon glyphicon-chevron-up" />
            </a> :
          null
        }
      </div>
    );
  }
}

export default MainPanel;