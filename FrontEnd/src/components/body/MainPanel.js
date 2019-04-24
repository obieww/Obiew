import React, {Component} from "react";
import Post from "@/components/common/Post.js";
import Card from "@/components/cards/Card.js";
import utils from "@/utils";
import "./MainPanel.less";

class MainPanel extends Component {
  constructor(props) {
    super(props);
    this.state = {
        hasNewMsg: true,
        showAnchor: false,
        cards: []
    };
    this.onShowNewMessage = this.onShowNewMessage.bind(this);
    this.onMousewheel = this.onMousewheel.bind(this);
    this.onCreateNewObiew = this.onCreateNewObiew.bind(this);
  }

  componentWillMount() {
    this.setState({
      cards: utils.defaultObiews,
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
    let cards = this.state.cards;
    cards.unshift(utils.defaultObiew(cards, "new message"))
    this.setState({
      cards: cards
    })
    this.setState({
      hasNewMsg: false
    });
    window.scrollTo(0, 0);
  }

  onCreateNewObiew(strMsg) {
    let cards = this.state.cards;
    cards.unshift(utils.defaultObiew(cards, strMsg));
    this.setState({
      cards: cards
    })
  }

  render() {
    const {
      cards,
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
          cards.map(card => <Card key={card.obiewId} card={card} />)
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