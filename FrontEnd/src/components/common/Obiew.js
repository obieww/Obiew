import React, {Component} from "react";
import utils from "@/utils";
import portrait from '../../images/person_default.png'
import "./Obiew.less";

class Obiew extends Component {
  render() {
    const {
      username,
      obiew,
      timestamp,
      rightBtns,
    } = this.props.info
    return (
      <div className="media">
        <div className="left">
          <img className="person-icon pa1 ba b--black-10" src={portrait} alt='portrait'/>
        </div>
        <div className="media-body">
          <p>
            <b className="remark-person-name f3 mb2">{username}</b>
            <div className="follow gray f4">Followed</div>
          </p>
          <p><span>{obiew}</span></p>
          <div className="remark-foot">
            <div className="remark-item-left">{utils.timeFormat(timestamp)}</div>
            {
              rightBtns.map((rightBtn, index) => (
                <React.Fragment key={index}>
                  <div className="remark-item-right" onClick={(e) => this.props.onClickRightBtn(rightBtn)}>
                    <span className={"glyphicon " + rightBtn.icon}></span>  {rightBtn.label}
                  </div>
                  {
                    index !== rightBtns.length - 1 ? 
                    <div className="remark-item-right split" /> :
                    null
                  }
                </React.Fragment>
              ))
            }
          </div>
        </div>
      </div>
    );
  }
}

export default Obiew;